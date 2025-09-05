# © 2023 Intel Corporation
#
# This software and the related documents are Intel copyrighted materials, and
# your use of them is governed by the express license under which they were
# provided to you ("License"). Unless the License provides otherwise, you may
# not use, modify, copy, publish, distribute, disclose or transmit this software
# or the related documents without Intel's prior written permission.
#
# This software and the related documents are provided as is, with no express or
# implied warranties, other than those that are expressly stated in the License.

import sys
import traceback
import asyncio
import unittest
import functools
import contextlib
import typing
from dataclasses import dataclass
import logging
from typing import Callable
import random

import simics
from simicsutils.host import is_windows

import snoop


__all__ = ('Break', 'Error', 'get_running_loop',
           'global_event_loop',
           'run_until_complete', 'create_task', 'call_soon',
           'wrap_future', 'wait_call',

           'wait',
           'trace',
           'Tracer',
           'CallbackScope',
           'timeout',
           'TaskGroup',
           )


def _caller_stack():
    return traceback.extract_stack()[:-2]

class Error(Exception):
    '''Raised for incorrect usage of `sloop`
    '''

class Break(Exception):
    '''Raised by `run_until_complete` when simulation was interrupted, e.g.
    when the end-user issued the `break` CLI command.
    '''


def _run_alone_wrapper(arg):
    (fn, stack) = arg
    try:
        fn()
    except BaseException as e:
        if stack is not None:
            print('*** Error in callback invoked from here:', file=sys.stderr)
            traceback.print_list(stack)
        raise e


def run_alone(fn, stack):
    '''Run fn in Global Context. If user errors can cause exceptions in
    fn, then 'stack' contains additional stack frames from the caller,
    as returned from traceback.extract_stack().'''
    simics.SIM_run_alone(_run_alone_wrapper, (fn, stack))


def run_alone_or_now(fun, *args):
    if simics.VT_is_oec_thread():
        fun(*args)
    else:
        run_alone(lambda: fun(*args), traceback.extract_stack()[:-2])


class EventLoop:
    # asyncio.EventLoop -> EventLoop
    running_loops = {}
    def __init__(self):
        self.asyncio_loop = asyncio.new_event_loop()
        asyncio.set_event_loop(self.asyncio_loop)

        self.asyncio_loop.set_debug(True)
        def exception_handler(loop, ctx):
            loop.default_exception_handler(ctx)
            msg = repr(ctx.get('exception') or ctx.get('message'))
            self.panic.append(
                f"Unexpected error {msg} in event loop")
        self.asyncio_loop.set_exception_handler(exception_handler)

        self._waiting_tasks = set()
        self._running_tasks = set()
        self._call_soons = set()
        self.running = False
        self.panic = []
        self.logger = logging.getLogger('sloop')

    def quiesced(self):
        '''return True if all tasks are suspended, waiting for sloop.Future
        objects'''
        assert simics.VT_is_oec_thread()
        return (not self.running
                and len(self._waiting_tasks) == len(self._running_tasks)
                and not self._call_soons)

    def _conditional_stop(self):
        assert simics.VT_is_oec_thread()
        if (len(self._waiting_tasks) == len(self._running_tasks)
            and not self._call_soons):
            self.asyncio_loop.stop()

    def _timeout_nag(self):
        for t in self._running_tasks:
            if t not in self._waiting_tasks:
                self.logger.warning(
                    'Task did not await sloop future within %s s. Use'
                    ' sloop.wrap_future() to wait for non-sloop futures.\n%s',
                    self.asyncio_loop.slow_callback_duration,
                    ''.join(s for frame in t.get_stack()
                            for s in traceback.format_stack(frame)))
        self.asyncio_loop.stop()

    def quiesce(self):
        assert simics.VT_is_oec_thread()
        if self.asyncio_loop.is_closed():
            # this may be called from a SIM_run_alone lingering after close
            return
        if not self.running:
            self.running_loops[self.asyncio_loop] = self
            self.running = True
            try:
                while (len(self._running_tasks) != len(self._waiting_tasks)
                       or self._call_soons):
                    if self.asyncio_loop.get_debug():
                        handle = self.asyncio_loop.call_later(
                            self.asyncio_loop.slow_callback_duration,
                            self._timeout_nag)
                    else:
                        handle = None
                    self.asyncio_loop.run_forever()

                    if handle:
                        handle.cancel()
            finally:
                self.running = False
                del self.running_loops[self.asyncio_loop]
            if self.panic:
                msg = ', '.join(self.panic)
                del self.panic[:]
                raise Error(f"Panic: {msg}")

    def create_future(self):
        return Future(self)

    def _on_task_done(self, task):
        self._running_tasks.remove(task)
        self._conditional_stop()

    def create_task(self, coro, *, name=None):
        if not asyncio.iscoroutine(coro):
            msg = "create_task: argument must be a coroutine"
            if asyncio.iscoroutinefunction(coro):
                msg += ". Try passing 'coro()' instead of 'coro'"
            raise Error(msg)
        asyncio_task = self.asyncio_loop.create_task(coro, name=name)
        asyncio_task.add_done_callback(self._on_task_done)
        self._running_tasks.add(asyncio_task)
        self._wake_up()
        return Task(self, asyncio_task)

    def run_until_complete(self, fut):
        '''Similar to `asyncio.EventLoop.run_until_complete`, but runs
        simulation until a coroutine or future is done.'''
        if not simics.VT_is_oec_thread():
            raise Error('run_until_complete(): not in Global Context')
        if simics.SIM_simics_is_running() or self.running:
            raise Error('run_until_complete(): Already running')
        task_created = False
        if isinstance(fut, Future):
            # Wrap the future in a coroutine, and wrap that in a task.
            # This allows us to reuse the mechanism for keeping track
            # of which futures wait for which tasks.
            if fut.done():
                return fut.result()
            async def coro(fut):
                return await fut
            task = self.create_task(coro(fut))
            task_created = True
        elif isinstance(fut, Task):
            task = fut
        elif asyncio.iscoroutine(fut):
            task = self.create_task(fut)
            task_created = True
        elif asyncio.iscoroutinefunction(fut):
            raise Error("run_until_complete: argument is coroutine function."
                        " Try passing 'coro()' instead of 'coro'")
        else:
            raise Error("run_until_complete: need coroutine or sloop.Future"
                        " or task created by sloop.create_task()")

        if not task.done():
            self.quiesce()
        def break_cb(*args):
            simics.SIM_break_simulation("task complete")
        if not task.done():
            task.add_done_callback(break_cb)
            simics.SIM_continue(0)
        if not task.done():
            task.remove_done_callback(break_cb)
            if task_created:
                task.cancel()
            raise Break(
                'coroutine did not finish due to unrelated'
                f' simulation break: {simics.VT_get_break_message()}')
        return task.result()

    async def wait_call(self, f, *args):
        cpl = self.create_future()
        @functools.wraps(f)
        def cb(_):
            try:
                cpl.set_result(f(*args))
            except BaseException as e:
                cpl.set_exception(e)
        simics.SIM_run_alone(cb, None)
        return await cpl

    def call_soon(self, callback, *args):
        self._call_soons.add(callback)
        def cb(callback, *args):
            self._call_soons.remove(callback)
            callback(*args)
        self.asyncio_loop.call_soon(cb, callback, *args)
        self._wake_up()

    def wrap_future(self, alien):
        '''Await an arbitrary future object'''
        wrapped = asyncio.wrap_future(alien, loop=self.asyncio_loop)
        ret = self.create_future()
        def copy_result(fut):
            assert fut.done()
            if ret.done():
                # Skip propagation if .cancel() was already called on
                # the wrapped future.  Note that cancellation is not
                # propagated into the alien future.
                return
            if fut.cancelled():
                ret.cancel()
            else:
                exc = fut.exception()
                if exc is None:
                    ret.set_result(fut.result())
                else:
                    ret.set_exception(exc)
        wrapped.add_done_callback(copy_result)
        alien.add_done_callback(
            lambda _: simics.SIM_run_alone(lambda _: ret._unwait(), None))
        return ret

    def _wake_up(self):
        if not self.running:
            run_alone(self.quiesce, traceback.extract_stack())

class Task:
    '''Wrapper around asyncio.Task, needed because we need some custom
    actions on __await__, which the return value of asyncio.create_task
    does not support.
    '''
    def __init__(self, loop, asyncio_task):
        self.asyncio_task = asyncio_task
        self._fut = loop.wrap_future(asyncio_task)
        asyncio_task.add_done_callback(lambda _: self._fut.cancel())
    def __str__(self):
        return str(self.asyncio_task)
    def __await__(self):
        return self._fut.__await__()
    def cancel(self):
        self.asyncio_task.cancel()
    def cancelled(self):
        return self.asyncio_task.cancelled()
    def done(self):
        return self.asyncio_task.done()
    def result(self):
        return self.asyncio_task.result()
    def exception(self):
        return self.asyncio_task.exception()
    def add_done_callback(self, cb):
        self.asyncio_task.add_done_callback(cb)
    def remove_done_callback(self, cb):
        self.asyncio_task.remove_done_callback(cb)

class Future(asyncio.Future):
    def __init__(self, loop):
        super().__init__(loop=loop.asyncio_loop)
        self.loop = loop
        self._waiting_tasks = set()

    def _unwait(self):
        assert simics.VT_is_oec_thread()
        for t in self._waiting_tasks:
            self.loop._waiting_tasks.remove(t)
        self._waiting_tasks.clear()
        self.loop._wake_up()

    def __await__(self):
        assert simics.VT_is_oec_thread() and self.loop.running
        if not self.done():
            task = asyncio.current_task()
            assert task in self.loop._running_tasks
            assert task not in self.loop._waiting_tasks
            assert task not in self._waiting_tasks
            self.loop._waiting_tasks.add(task)
            self._waiting_tasks.add(task)
            self.loop._conditional_stop()
        ret = yield from super().__await__()
        return ret

    def cancel(self, msg=None):
        if not simics.VT_is_oec_thread():
            raise Error('cancel must be called in Global Context')
        try:
            return super().cancel(msg)
        finally:
            self._unwait()

    def set_result(self, result):
        if not simics.VT_is_oec_thread():
            raise Error('set_result must be called in Global Context')
        # Need synchronous set_result: it is reasonable to expect
        # x = Future() ; indirectly_set_result(x) ; assert x.done()
        # from within a coroutine, because you have that in asyncio
        # TODO: test
        super().set_result(result)
        self._unwait()

    def set_exception(self, exc):
        if not simics.VT_is_oec_thread():
            raise Error('set_exception must be called in Global Context')
        super().set_exception(exc)
        self._unwait()


_global_event_loop = EventLoop()
def global_event_loop():
    '''The default `sloop` event loop'''
    return _global_event_loop


def run_until_complete(fut):
    '''Similar to `asyncio.EventLoop.run_until_complete`, but runs
    simulation until a coroutine, future or task is done. If the
    argument is a coroutine, then the coroutine is scheduled as a task
    within the `global_event_loop` sloop event loop. May only be called
    while simulation is stopped, from Global Context.'''
    loop = (fut.loop if isinstance(fut, Future)
            else fut._fut.loop if isinstance(fut, Task)
            else global_event_loop())
    return loop.run_until_complete(fut)


def wrap_future(fut):
    '''Similar to `asyncio.wrap_future`, wrap a future-like object
    so it can be awaited within a sloop loop.

    Mainly useful to wrap the results of higher-order `asyncio` coroutines,
    e.g. `sloop.wrap_future(asyncio.gather(...))` or `map(sloop.wrap_future,
    asyncio.as_completed(...))`.

    Raises `RuntimeError` if not called
    from a `sloop` corotuine.
    '''
    return get_running_loop().wrap_future(fut)


async def wait_call(fun, *args):
    '''Similar to `asyncio.to_thread`, run a slow function
    outside the Sloop event loop and await the result.

    Like `asyncio`, `sloop` emits a warning if a coroutine takes too
    long between waits; wrapping a call in `wait_call` silences
    this. However, unlike `asyncio.to_thread`, `wait_call` schedules
    the function to run in Global Context rather than in a separate
    thread, so it will still block the progress of simulation. Thus, the
    motivation for the function is to allow preservation of asyncio's
    convention that expensive code should not be run within coroutines.

    Raises `RuntimeError` if not called from a `sloop` corotuine.
    '''
    return await get_running_loop().wait_call(fun, *args)


def call_soon(fun, *args):
    '''Like `asyncio.call_soon`, but must be run from a sloop task'''
    return get_running_loop().call_soon(fun, *args)


def create_task(coro, *, name=None):
    '''Like `asyncio.create_task`, create a task in the currently
    running `sloop` event loop. Raises `RuntimeError` if not called
    from a `sloop` corotuine.'''
    return get_running_loop().create_task(coro, name=name)


def get_running_loop():
    '''Like `asyncio.get_running_loop`, but must be run from a task
    created by Sloop's `create_task`.'''
    asyncio_loop = asyncio.get_running_loop()
    if asyncio_loop not in EventLoop.running_loops:
        raise RuntimeError('current event loop is not a sloop event loop')
    return EventLoop.running_loops[asyncio_loop]


def sloop_is_running():
    try:
        get_running_loop()
    except RuntimeError:
        return False
    return True


class TestLoop(unittest.TestCase):
    def setUp(self):
        self.clock = simics.SIM_create_object(
            'clock', 'clock', [['freq_mhz', 1]])
        self.loop = EventLoop()

    def tearDown(self):
        simics.SIM_delete_object(self.clock)
        self.loop.asyncio_loop.close()

    def delayed_call(self, seconds, fun):
        def call_and_cancel(_):
            try:
                fun()
            finally:
                handle.cancel()
        handle = snoop.add_callback_simple(
            snoop.Seconds(self.clock, seconds),
            lambda _: simics.SIM_run_alone(call_and_cancel, None))

    def test_basic_coroutine(self):
        loop = self.loop
        result = []
        cpl = loop.create_future()
        async def coro(arg):
            result.append(arg)
            result.append(await cpl)
            result.append(None)
            result.append(await cpl)

        task = loop.create_task(coro(3))
        self.assertEqual(result, [])
        simics.SIM_process_pending_work()
        self.assertEqual(result, [3])
        del result[:]
        simics.SIM_process_pending_work()
        self.assertEqual(result, [])
        cpl.set_result(5)
        loop.run_until_complete(task)
        self.assertEqual(result, [5, None, 5])
        del result[:]

        loop.run_until_complete(coro(4))
        self.assertEqual(result, [4, 5, None, 5])
        del result[:]

        assert loop.quiesced()

    def test_create_task(self):
        # The name= arg is exists in both method and global versions, is
        # kw-only, and affects str
        async def coro2():
            pass
        async def coro():
            c = coro2()
            with self.assertRaises(TypeError):
                # fisketur[call-args]
                create_task(c, 'whale')
            t = create_task(c, name='whale')
            assert 'whale' in str(t)
            await t
        c = coro()
        with self.assertRaises(TypeError):
            self.loop.create_task(c, 'whale')
        t = self.loop.create_task(c, name='monkey')
        self.loop.run_until_complete(t)
        assert 'monkey' in str(t)

        # global create_task may only be called from within a coroutine
        async def coro3():
            return 3
        c = coro3()
        with self.assertRaises(RuntimeError):
            create_task(c)
        async def coro1():
            # await coro to avoid gc nag from event queue
            return await create_task(c)
        self.assertEqual(run_until_complete(coro1()), 3)

    def test_cancel(self):
        fut = self.loop.create_future()
        x = []
        async def coro():
            x.append(0)
            await fut
            x.append(1)
            return 4
        t = self.loop.create_task(coro())
        simics.SIM_process_pending_work()
        self.assertEqual(x, [0])
        fut.cancel()
        simics.SIM_process_pending_work()
        self.assertEqual(x, [0])
        self.assertTrue(t.cancelled())

    def _test_trigger_order(self):
        class CowException(Exception): pass
        async def broken():
            raise CowException('horse')

        import conf
        old = conf.sim.stop_on_error
        conf.sim.stop_on_error = False
        try:
            with self.assertRaisesRegex(
                    Error, "Task crashed: CowException[(]'horse'[)]"):
                self.loop.run_until_complete(broken())
        finally:
            conf.sim.stop_on_error = old

        assert self.loop.quiesced()

    def test_call_soon(self):
        ls = []
        def fn(x, y):
            x.append(y)
        self.loop.call_soon(*([self.loop.call_soon] * 5 + [fn, ls, 4]))
        simics.SIM_process_pending_work()
        self.assertEqual(ls, [4])
        del ls[:]
        with self.assertRaises(RuntimeError):
            call_soon(fn, ls, 5)
        async def coro():
            call_soon(fn, ls, 6)
        run_until_complete(coro())
        self.assertEqual(ls, [6])

    def test_run_until_complete(self):
        import concurrent
        # non-asyncio futures are not accepted
        fut = concurrent.futures.Future()
        with self.assertRaises(Error):
            self.loop.run_until_complete(fut)
        # asyncio futures are accepted
        fut = self.loop.create_future()
        self.delayed_call(1, lambda: fut.set_result(1))
        self.assertEqual(self.loop.run_until_complete(fut), 1)
        self.assertEqual(self.loop.run_until_complete(fut), 1)

        # coroutine functions do not work, but coroutine objects do
        fut2 = self.loop.create_future()
        async def coro():
            await fut2
        self.delayed_call(1, lambda: fut2.set_result(1))
        with self.assertRaisesRegex(
                Error, r"Try passing 'coro\(\)' instead of 'coro'"):
            self.loop.run_until_complete(coro)
        self.loop.run_until_complete(coro())

        # unexpected simulation break causes a Break exception
        fut3 = self.loop.create_future()
        async def coro2():
            await fut3
        self.delayed_call(1, lambda: simics.SIM_break_simulation("hello"))
        with self.assertRaisesRegex(Break, "hello"):
            self.loop.run_until_complete(coro2())
        fut3.cancel()
        self.loop.quiesce()

    def test_panic(self):
        fut = self.loop.create_future()
        class SheepException(Exception): pass
        @fut.add_done_callback
        def raise_sheep(fut):
            raise SheepException('baah')
        async def coro():
            await fut
        self.loop.create_task(coro())
        self.loop.quiesce()
        fut.set_result(None)
        with self.assertLogs(
                logging.getLogger('asyncio'), logging.WARNING) as captured:
            with self.assertRaisesRegex(
                    Error, "Panic: .*SheepException[(]'baah'[)]"):
                self.loop.quiesce()
        self.assertEqual(len(captured.records), 1, captured.records)
        self.assertIn('raise_sheep', captured.records[0].getMessage(),
                      captured.records)

    def test_wrap_future(self):
        import concurrent.futures
        import time
        self.loop.asyncio_loop.slow_callback_duration = 0.01
        with concurrent.futures.ThreadPoolExecutor(max_workers=1) as e:
            # Wrapping using asyncio.wrap_future gives a warning
            r = []
            def wait():
                # hack: busy-wait long enough to see the first warning
                while not r:
                    time.sleep(0.001)
            fut = e.submit(wait)
            async def bad_sleep():
                await asyncio.wrap_future(fut)
            with self.assertLogs(
                    logging.getLogger('sloop'), logging.WARNING) as captured:
                r = captured.records
                self.loop.run_until_complete(bad_sleep())
            self.assertTrue(captured.records)
            for record in captured.records:
                self.assertIn('bad_sleep', captured.records[0].getMessage(),
                              record)

        self.loop.asyncio_loop.slow_callback_duration = 1
        # Wrapping using sloop.wrap_future works fine.
        fut2 = concurrent.futures.Future()
        self.delayed_call(1, lambda: fut2.set_result(None))
        async def good_sleep():
            await self.loop.wrap_future(fut2)
        with self.assertLogs() as captured:
            self.loop.run_until_complete(good_sleep())
            # workaround for not having assertNoLogs
            # (will be added in py 3.10)
            logging.getLogger('asyncio').warning('')
        self.assertEqual(len(captured.records), 1, captured.records)

        # Exceptions are propagated from the alien future
        class SheepException(Exception): pass
        fut3 = concurrent.futures.Future()
        self.delayed_call(1, lambda: fut3.set_exception(SheepException()))
        async def exceptional_sleep():
            with self.assertRaises(SheepException):
                await self.loop.wrap_future(fut3)
            return 6
        self.assertEqual(
            self.loop.run_until_complete(exceptional_sleep()), 6)

        # Cancellation is propagated from the alien future,
        # but wakeup only happens after SIM_process_pending_work()
        fut4 = concurrent.futures.Future()
        async def cancelled_sleep():
            await self.loop.wrap_future(fut4)
        t = self.loop.create_task(cancelled_sleep())
        self.loop.quiesce()
        ok = fut4.cancel()
        assert ok
        simics.SIM_process_pending_work()
        self.assertTrue(t.cancelled())

        # Exercise the ret.done() check in wrap_future.copy_result
        fut5 = concurrent.futures.Future()
        wrapped = self.loop.wrap_future(fut5)
        wrapped.cancel()
        fut5.set_result(None)
        async def nop():
            pass
        self.loop.run_until_complete(nop())

        fut6 = concurrent.futures.Future()
        with self.assertRaises(RuntimeError):
            wrap_future(fut6)
        async def coro():
            return await wrap_future(fut6)
        fut6.set_result(7)
        self.assertEqual(self.loop.run_until_complete(coro()), 7)


    def test_wait_call(self):
        # asyncio complains when a callback exceeds
        # slow_callback_duration, and wait_call silences that.
        import time
        timeout = 0.1 if is_windows() else 0.01
        self.loop.asyncio_loop.slow_callback_duration = timeout
        fut = self.loop.create_future()
        fut.set_result(None)
        async def slow():
            time.sleep(timeout * 2)
        with self.assertLogs(
                logging.getLogger('asyncio'), logging.WARNING) as captured:
            self.loop.run_until_complete(self.loop.create_task(slow()))
        self.assertEqual(len(captured.records), 1, captured.records)
        async def slow_wrapped():
            await self.loop.wait_call(time.sleep, timeout * 2)
        with self.assertLogs() as captured:
            self.loop.run_until_complete(self.loop.create_task(slow_wrapped()))
            # workaround for not having assertNoLogs
            # (will be added in py 3.10)
            logging.getLogger('asyncio').warning('')
        self.assertEqual(len(captured.records), 1, captured.records)

        with self.assertRaises(RuntimeError):
            asyncio.run(wait_call(lambda: 3))
        async def coro():
            return await wait_call(lambda: 4)
        self.assertEqual(run_until_complete(coro()), 4)

    def test_await_task(self):
        fut = self.loop.create_future()
        async def coro():
            await fut
        task = self.loop.create_task(coro())

        self.delayed_call(1, lambda: fut.set_result(None))
        async def main():
            await task#loop.wrap_future(task)
        self.loop.run_until_complete(main())


T = typing.TypeVar('T')


async def wait(s: snoop.Snooper):
    '''Wait for the next time a snooper yields.'''
    fut = get_running_loop().create_future()
    cancelled = False
    def done(r):
        nonlocal cancelled
        # we only await the first yielded value; if more values are
        # yielded while waiting for cancellation in run_alone, then
        # drop these.
        if not cancelled:
            cancelled = True
            @run_alone_or_now
            def stop():
                handle.cancel()
                if not fut.cancelled():
                    fut.set_result(r)
    def raise_exception(exc):
        nonlocal cancelled
        # suppress exception if is observed after the cancellation
        # was initiated. Otherwise, propagate it.
        if not cancelled:
            cancelled = True
            @run_alone_or_now
            def stop():
                handle.cancel()
                if not fut.cancelled():
                    fut.set_exception(exc)
    handle = s.add_callback(done, raise_exception)
    try:
        return await fut
    finally:
        if not cancelled:
            handle.cancel()


class _SnooperTest(unittest.TestCase):
    calls = []
    def flush(self):
        x = self.calls[:]
        del self.calls[:]
        return x

    class Handle(snoop.Handle):
        def cancel(self):
            _SnooperTest.calls.append(('cancel', self))
    class Snooper(snoop.Snooper):
        cbs = []
        def __init__(self, *args, **kwargs):
            self.args = args
            assert not kwargs
        def add_callback(self, yield_value, yield_exc):
            handle = _SnooperTest.Handle()
            self.cbs.append((yield_value, yield_exc, self.args, handle))
            return handle
        def exec_context(self):
            return snoop.GlobalContext()


class TestWait(_SnooperTest):
    def test_await(self):
        async def coro():
            self.calls.append(('await', await wait(self.Snooper(1))))
        task = global_event_loop().create_task(coro())
        assert self.calls == []
        simics.SIM_process_pending_work()
        [(cb, ctx, args, handle)] = self.Snooper.cbs
        self.assertEqual(args, (1,))
        del self.Snooper.cbs[:]
        # works like a normal snoop when callback is called
        cb(10)
        simics.SIM_process_pending_work()
        self.assertEqual(self.flush(), [('cancel', handle), ('await', 10)])
        self.assertTrue(task.done())

        done = False
        class TestExc(Exception): pass
        async def coro2():
            try:
                self.calls.append(('await', await wait(self.Snooper(2))))
            except TestExc:
                nonlocal done
                done = True
        task = global_event_loop().create_task(coro2())
        simics.SIM_process_pending_work()
        [(cb, exc_cb, args, handle)] = self.Snooper.cbs
        self.assertEqual(args, (2,))
        del self.Snooper.cbs[:]
        # exception is propagated to the waiting coroutine
        exc_cb(TestExc())
        simics.SIM_process_pending_work()
        self.assertEqual(self.flush(), [('cancel', handle)])
        self.assertTrue(task.done())
        self.assertTrue(done)

    def test_run_until_complete_break(self):
        clk = simics.SIM_create_object('clock', 'clk', freq_mhz=1)
        try:
            never = global_event_loop().create_future()
            async def break_soon():
                await wait(snoop.Seconds(clk, 0.01))
                simics.SIM_break_simulation("hello")
                await never
            with self.assertRaisesRegex(Break, 'hello'):
                run_until_complete(break_soon())
        finally:
            simics.SIM_delete_object(clk)

    def test_seconds(self):
        '''Test that await works on callbacks triggered in cell context'''
        clk = simics.SIM_create_object('clock', 'clk', [['freq_mhz', 1]])
        try:
            # Basic await
            x = []
            ms = snoop.Seconds(clk) / 1000
            async def m():
                x.append(1)
                await wait(ms)
                x.append(2)
            global_event_loop().create_task(m())
            simics.SIM_process_pending_work()
            self.assertEqual(x, [1])
            del x[:]
            simics.SIM_continue(15000)
            self.assertEqual(x, [2])

            # if a sequence of values are yielded rapidly, only the first
            # one is reacted upon.
            async def coro():
                self.calls.append(await wait(self.Snooper()))
                try:
                    self.calls.append(await wait(self.Snooper()))
                except Exception as e:
                    self.calls.append(e)
                else:
                    self.calls.append('no exception')
            global_event_loop().create_task(coro())
            simics.SIM_process_pending_work()
            [(cb, exc_cb, _, handle)] = self.Snooper.cbs
            del self.Snooper.cbs[:]
            e1 = Exception("1")
            def tick1(_):
                cb(3)
                cb(4)
                exc_cb(e1)
                self.calls.append('tick1')
            h = snoop.add_callback_simple(ms, tick1)
            simics.SIM_continue(1005)
            self.assertEqual(self.flush(), ['tick1', ('cancel', handle), 3])
            # second wait started already
            [(cb, exc_cb, _, handle)] = self.Snooper.cbs
            del self.Snooper.cbs[:]
            h.cancel()
            # Similarly, if the first thing that happens is an exception,
            # then it shadows subsequent values and exceptions.
            e2 = Exception("2")
            def tick2(_):
                exc_cb(e2)
                cb(5)
                exc_cb(e1)
                self.calls.append('tick2')
            h = snoop.add_callback_simple(ms, tick2)
            simics.SIM_continue(1005)
            self.assertEqual(self.flush(), ['tick2', ('cancel', handle), e2])
            h.cancel()
        finally:
            simics.SIM_delete_object(clk)


class SnoopAsyncIterator:
    def __init__(self):
        self._fut = None
        self.q = []
    def __aiter__(self):
        return self
    async def __anext__(self):
        assert self._fut is None, 'already awaiting'
        if self.q:
            result = self.q.pop(0)
            if isinstance(result, tuple):
                (ret,) = result
                return ret
            else:
                assert isinstance(result, Exception)
                raise result
        else:
            assert sloop_is_running()
            self._fut = get_running_loop().create_future()
            try:
                return await self._fut
            finally:
                self._fut = None
    def next(self, arg):
        '''called from snoop's thread when an element is produced'''
        if self._fut is None:
            self.q.append((arg,))
        else:
            fut = self._fut
            self._fut = None
            @run_alone_or_now
            def set_result():
                if not fut.cancelled():
                    fut.set_result(arg)
    def stop(self, arg):
        '''called from snoop's thread when iteration finishes'''
        self.raise_exception(StopAsyncIteration())
    def raise_exception(self, exc):
        '''called from snoop's thread when an exception is produced'''
        if self._fut is None:
            self.q.append(exc)
        else:
            fut = self._fut
            self._fut = None
            @run_alone_or_now
            def set_exception():
                if not fut.cancelled():
                    fut.set_exception(exc)


class Tracer:
    '''Asynchronous context manager that converts a snooper into an
    asynchronous iterator of yielded values. The asynchronous iterator is
    returned by `__enter__`, so typical usage looks like this:
    ```
    async with Tracer(SomeSnooper(...)) as t:
        async for value in t:
            ...
    ```'''
    def __init__(self, snooper: snoop.Snooper, until: snoop.Snooper=None):
        '''`snooper` is the snooper whose values to collect. `until`
        specifies when to stop iteration; the default is to continue
        indefinitely.'''
        if until is not None:
            # TODO: context validation not tested
            contexts = (snooper.exec_context(), until.exec_context())
            if (any(isinstance(ctx, snoop.ThreadedContext) for ctx in contexts)
                or (all(isinstance(ctx, snoop.CellContext) for ctx in contexts)
                    and (contexts[0].cell != contexts[1].cell
                         or contexts[0].cell is None))):
                raise Error('Incompatible execution contexts'
                            f' for {snooper} and {until}')
        self.snooper = snooper
        self.until = until
        self.handle = None
        self.until_handle = None

    def __enter__(self):
        assert simics.VT_is_oec_thread()
        aiter = SnoopAsyncIterator()
        self.handle = self.snooper.add_callback(
            aiter.next, aiter.raise_exception)
        if self.until:
            self.until_handle = self.until.add_callback(
                aiter.stop, aiter.raise_exception)
        return aiter

    def __exit__(self, *args):
        self.handle.cancel()
        if self.until_handle:
            self.until_handle.cancel()


async def trace(snooper, until):
    '''Collect all values yielded by `snooper` until the first time
    `until` yields, and return a list of these values.'''
    with Tracer(snooper, until) as t:
        return [x async for x in t]


class TestTrace(_SnooperTest):
    def test_aiter(self):
        class TestExc(Exception): pass
        async def coro():
            with Tracer(self.Snooper(23)) as t:
                while True:
                    try:
                        async for x in t:
                            self.calls.append(x)
                    except TestExc:
                        self.calls.append(TestExc)
                    else:
                        return 5
        t = global_event_loop().create_task(coro())
        simics.SIM_process_pending_work()
        [(cb, exc_cb, args, handle)] = self.Snooper.cbs
        del self.Snooper.cbs[:]
        self.assertEqual(args, (23,))
        cb(1)
        cb(2)
        simics.SIM_process_pending_work()
        self.assertEqual(self.flush(), [1, 2])
        cb(3)
        exc_cb(TestExc())
        cb(4)
        simics.SIM_process_pending_work()
        self.assertEqual(self.flush(), [3, TestExc, 4])
        exc_cb(TestExc())
        simics.SIM_process_pending_work()
        self.assertEqual(self.flush(), [TestExc])
        exc_cb(StopAsyncIteration())
        simics.SIM_process_pending_work()
        self.assertEqual(self.flush(), [('cancel', handle)])
        self.assertTrue(t.done())
        self.assertEqual(t.result(), 5)
        assert not self.Snooper.cbs

    def test_tracer_until(self):
        async def coro():
            with Tracer(self.Snooper(29), until=self.Snooper(31)) as t:
                return [x async for x in t]
        t = global_event_loop().create_task(coro())
        simics.SIM_process_pending_work()
        [(next, next_ctx, next_args, next_handle),
         (until, until_ctx, until_args, until_handle)] = self.Snooper.cbs
        del self.Snooper.cbs[:]
        self.assertEqual((next_args, until_args), ((29,), (31,)))
        next(1)
        next(2)
        simics.SIM_process_pending_work()
        until(4)
        next(3)   # silently discarded
        until(5)  # silently discarded
        self.assertFalse(t.done())
        simics.SIM_process_pending_work()
        self.assertTrue(t.done())
        self.assertEqual(t.result(), [1, 2])
        self.assertEqual(set(self.flush()), {('cancel', next_handle),
                                             ('cancel', until_handle)})
        assert not self.Snooper.cbs

    def test_trace(self):
        async def coro():
            return await trace(self.Snooper(41), until=self.Snooper(43))
        t = global_event_loop().create_task(coro())
        simics.SIM_process_pending_work()
        [(next, next_ctx, next_args, next_handle),
         (until, until_ctx, until_args, until_handle)] = self.Snooper.cbs
        del self.Snooper.cbs[:]
        self.assertEqual((next_args, until_args), ((41,), (43,)))
        next(1)
        simics.SIM_process_pending_work()
        next(2)
        until(4)
        next(3)   # silently discarded
        until(5)  # silently discarded
        self.assertFalse(t.done())
        simics.SIM_process_pending_work()
        self.assertTrue(t.done())
        self.assertEqual(t.result(), [1, 2])
        self.assertEqual(set(self.flush()), {('cancel', next_handle),
                                             ('cancel', until_handle)})
        assert not self.Snooper.cbs


class CallbackScope(contextlib.AbstractAsyncContextManager):
    '''Reusable asynchronous context manager that subscribes a
    callback to a snooper while entered.
    '''
    unique = 0
    @dataclass(eq=False)
    class Canceller:
        task : asyncio.Task
        loop : EventLoop
        ident : typing.Optional[str] = None
        exc : typing.Optional[Exception] = None
        # If an exception is signalled just before the callback scope exits,
        # then the delay incurred from run_alone may cause self.propagate
        # to happen after scope is exited, meaning the fallthrough
        # won the race. In this case, done is set to True to suppress
        # cancellation.
        done : bool = False
        def yield_exc(self, exc):
            if not self.done:
                self.exc = exc
                run_alone_or_now(self.propagate)
        def propagate(self):
            if not self.done:
                # hideous hack: create a unique identifier so that
                # the aexit handler can catch only that, in case of nested
                # scopes
                self.ident = f'callback-scope-{CallbackScope.unique}'
                CallbackScope.unique += 1
                self.task.cancel(self.ident)

    def __init__(self, snp: snoop.Snooper[T], cb: Callable[[T], None]):
        '''The callback `cb` is subscribed to `snp` while entered.'''
        self.snooper = snp
        # ThreadedContext could give race on ExcBridge.exc
        ctx = self.snooper.exec_context()
        assert (isinstance(ctx, snoop.GlobalContext)
                or (isinstance(ctx, snoop.CellContext)
                    and ctx.cell is not None))
        self.cb = cb
        self.handle = None
        self.canceller = None
        self.loop = get_running_loop()

    async def __aenter__(self):
        assert self.handle is None
        self.canceller = self.Canceller(asyncio.current_task(), self.loop)
        self.handle = self.snooper.add_callback(
            self.cb, self.canceller.yield_exc)
        return None

    async def __aexit__(self, exc_type, exc_value, cb):
        self.handle.cancel()
        self.handle = None
        canceller = self.canceller
        canceller.done = True
        if canceller.ident:
            if (isinstance(exc_value, asyncio.CancelledError)
                and exc_value.args == (canceller.ident,)):
                raise canceller.exc


def timeout(snooper):
    '''Similar to `asyncio.timeout` in Python 3.11, returns an
    asynchronous context manager that interrupts the block by raising a
    `TimeoutError` exception if it does not finish in time. Unlike
    `asyncio.timeout`, the timeout is specified by a snooper object,
    typically an instance of `sloop.Seconds`.
    '''
    def cancel(*args):
        if not scope.canceller.exc:
            scope.canceller.exc = TimeoutError()
            # TODO test doesn't cover the run_alone_or_now requirement
            run_alone_or_now(scope.loop.call_soon, scope.canceller.propagate)
    scope = CallbackScope(snooper, cancel)
    return scope


class TestCallbackScope(_SnooperTest):
    def setUp(self):
        self.loop = EventLoop()

    def tearDown(self):
        self.loop.asyncio_loop.close()

    def test_callback_scope(self):
        def callback(*args):
            self.calls.append(args)
        snp = self.Snooper(3)
        inside = self.loop.create_future()
        never = self.loop.create_future()
        class TestExc(Exception): pass
        async def coro():
            with self.assertRaises(TestExc):
                async with CallbackScope(snp, callback):
                    inside.set_result(None)
                    await never
            return 17

        task = self.loop.create_task(coro())
        self.loop.run_until_complete(inside)
        [(cb, exc_cb, args, handle)] = self.Snooper.cbs
        del self.Snooper.cbs[:]
        self.assertEqual(args, (3,))
        cb(7)
        self.assertEqual(self.flush(), [(7,)])
        exc_cb(TestExc())
        self.loop.quiesce()
        self.assertEqual(self.loop.run_until_complete(task), 17)
        self.assertEqual(self.flush(), [('cancel', handle)])

    def test_nested_callback_scope(self):
        '''when nesting callback scopes, an exception in the outer one's
        snoop evades the inner scope, and is propagated to the exit
        handler of the outer scope'''
        class TestExc(Exception): pass
        never = self.loop.create_future()
        def callback(*args):
            self.calls.append(args)
        outer_snoop = self.Snooper(3)
        inner_snoop = self.Snooper(5)
        async def nested_coro():
            try:
                async with CallbackScope(outer_snoop, callback):
                    try:
                        async with CallbackScope(inner_snoop, callback):
                            await never
                    except TestExc:
                        return 'inner'
            except TestExc:
                return 'outer'
        task = self.loop.create_task(nested_coro())
        self.loop.quiesce()
        [(_, outer_exc_cb, outer_args, outer_handle),
         (_, _, inner_args, inner_handle)] = self.Snooper.cbs
        assert (outer_args, inner_args) == ((3,), (5,))
        del self.Snooper.cbs[:]
        outer_exc_cb(TestExc())
        self.assertEqual(self.loop.run_until_complete(task), 'outer')
        self.assertEqual(set(self.flush()),
                         {('cancel', inner_handle), ('cancel', outer_handle)})

    def test_callback_scope_exit_race(self):
        def callback(*args):
            self.calls.append(args)
        snp = self.Snooper(3)
        fut = None
        class TestExc(Exception): pass
        async def coro():
            nonlocal fut
            fut = self.loop.create_future()
            try:
                async with CallbackScope(snp, callback):
                    await fut
                    self.calls.append('fallthrough')
            except TestExc:
                self.calls.append('exception')
                #assert futs.one.cancelled()
            return 7

        # raise just after resumption
        task = self.loop.create_task(coro())
        self.loop.quiesce()
        [(cb, exc_cb, args, handle)] = self.Snooper.cbs
        del self.Snooper.cbs[:]
        fut.set_result(None)
        self.loop.call_soon(exc_cb, TestExc())
        self.loop.quiesce()
        self.assertEqual(self.flush(), ['fallthrough', ('cancel', handle)])
        self.assertEqual(self.loop.run_until_complete(task), 7)

        # resume just after exception
        task = self.loop.create_task(coro())
        self.loop.quiesce()
        [(cb, exc_cb, args, handle)] = self.Snooper.cbs
        del self.Snooper.cbs[:]
        fut.set_result(None)
        exc_cb(TestExc())
        self.loop.quiesce()
        self.loop.run_until_complete(fut)
        self.assertEqual(self.flush(), [('cancel', handle), 'exception'])
        self.assertEqual(self.loop.run_until_complete(task), 7)

    def test_timeout(self):
        snp = self.Snooper()
        never = self.loop.create_future()
        async def coro():
            try:
                async with timeout(snp):
                    await never
            except TimeoutError:
                return 53

        t = self.loop.create_task(coro())
        self.loop.quiesce()
        [(cb, exc_cb, args, handle)] = self.Snooper.cbs
        del self.Snooper.cbs[:]
        cb()
        self.assertEqual(self.loop.run_until_complete(t), 53)
        self.assertEqual(self.flush(), [('cancel', handle)])

if (sys.version_info.major, sys.version_info.minor) in {(3, 9), (3, 10)}:
    def _format_exception_group(exc):
        # The traceback printing of 3.9 does not honour exception groups;
        # as a workaround, print the sub-exception tracebacks to make
        # printed tracebacks usable in the common case
        return (
            f'{exc.message} ({len(exc.exceptions)} sub-exceptions):\n'
            + '\n'.join([
                ''.join(
                    traceback.format_exception(type(e), e, e.__traceback__))
                for e in exc.exceptions]))

    class BaseExceptionGroup(BaseException):
        '''Limited back-port of the built-in `BaseExceptionGroup`
        class from Python 3.11. Provides the `exceptions` attribute
        but no methods.

        Python's traceback printer is not aware of the tracebacks of
        the sub-exceptions, so these are printed as part of `__str__`
        in order to yield comprehensive tracebacks.

        When `sloop` is used with Python 3.11 or newer,
        `sloop.BaseExceptionGroup` evaluates to the built-in
        `BaseExceptionGroup` class.
        '''
        def __init__(self, message, exceptions):
            super().__init__(message, exceptions)
            self.message = message
            self.exceptions = exceptions
        def __str__(self):
            return _format_exception_group(self)
        def __repr__(self):
            return f'BaseExceptionGroup{(self.message, self.exceptions)}'

    class ExceptionGroup(Exception):
        '''Limited back-port of the built-in `ExceptionGroup` class from Python
        3.11. Provides the `exceptions` attribute but no methods.

        Python's traceback printer is not aware of the tracebacks of
        the sub-exceptions, so these are printed as part of `__str__`
        in order to yield comprehensive tracebacks.

        When `sloop` is used with Python 3.11 or newer,
        `sloop.ExceptionGroup` evaluates to the built-in
        `ExceptionGroup` class.
        '''
        def __init__(self, message, exceptions):
            super().__init__()
            self.message = message
            self.exceptions = exceptions
        def __str__(self):
            return _format_exception_group(self)
        def __repr__(self):
            return f'ExceptionGroup{(self.message, self.exceptions)}'
    __all__ += (
           'BaseExceptionGroup',
           'ExceptionGroup',
        )
else:
    # fisketur[self-assign]
    BaseExceptionGroup = BaseExceptionGroup
    # fisketur[self-assign]
    ExceptionGroup = ExceptionGroup

class TaskGroup(contextlib.AbstractAsyncContextManager):
    '''Provides a `sloop` compatible implementation of the
    `asyncio.TaskGroup` class available in Python 3.11 and newer; see
    `asyncio` documentation for details. In cases where
    `asyncio.TaskGroup` would raise an `ExceptionGroup` or
    `BaseExceptionGroup` exception, and `sloop` is used from Python
    3.10, the `sloop.TaskGroup` class instead raises a
    `sloop.ExceptionGroup` or `sloop.BaseExceptionGroup` exception,
    respectively.

    '''
    def __init__(self):
        self._tasks = []
        self._task = None
        self._loop = None
        self._spawn_futures: dict[Task, Future] = {}
        self._newly_finished_tasks = []
        # True while the mother task is in `__aexit__`.
        # In particular, CancelledError during shutdown is interpreted
        # as a subtask signalling that it has raised an exception.
        self._shutting_down = False
        self._finish_ping: typing.Optional[Future] = None
        self._first = True

    def create_task(self, coro):
        if self._shutting_down:
            raise RuntimeError('TaskGroup is shutting down')
        if not self._loop:
            if self._first:
                raise RuntimeError('TaskGroup has not been entered')
            else:
                raise RuntimeError('TaskGroup is finished')
        @functools.wraps(coro)
        async def wrapper():
            self._spawn_futures.pop(task).set_result(None)
            try:
                return await coro
            except asyncio.CancelledError:
                raise
            except BaseException:
                if not self._shutting_down:
                    # Mother task is awaiting something amidst the
                    # with block.  Interrupt mother task by cancelling
                    # the task (because this is the only available way
                    # to interrupt an await from outside), but set
                    # `_shutting_down` to signal that this is not
                    # intended as a cancellation.
                    # Note that if someone else cancels the mother
                    # task at the same time, with a genuine intent to
                    # cancel the task, then that cancellation will be
                    # replaced with this exception. I think this is consistent
                    # with the docs of asyncio.TaskGroup.
                    self._shutting_down = True
                    self._task.cancel()
                raise
            finally:
                self._tasks.remove(task)
                self._newly_finished_tasks.append(task)
                if (self._finish_ping is not None
                      and not self._finish_ping.done()):
                    self._finish_ping.set_result(None)
        task = self._loop.create_task(wrapper())
        self._tasks.append(task)
        self._spawn_futures[task] = self._loop.create_future()
        return task

    async def __aenter__(self):
        if not self._first:
            raise RuntimeError('TaskGroup has been already entered')
        self._first = False
        self._loop = get_running_loop()
        self._task = asyncio.current_task()
        return self

    async def __aexit__(self, exc_type, exc, exc_tb):
        # if any newly spawned task hasn't started yet,
        # then wait for its wrapper function to start.
        # Without this, some bookkeeping invariants may get out of sync.
        while self._spawn_futures:
            t = next(iter(self._spawn_futures))
            if t.done():
                del self._spawn_futures[t]
            else:
                await self._spawn_futures[t]
        # Let a CancelledError propagate as-is, unless the
        # CancelledError is an attempt from the `wrapper` function in
        # `create_task` to interrupt an await in its mother task.
        reraise = (not self._shutting_down
                   and isinstance(exc, asyncio.CancelledError))
        exceptions = []
        tasks_cancelled = False
        if exc:
            if isinstance(exc, (KeyboardInterrupt, SystemExit)):
                return False
            if not isinstance(exc, asyncio.CancelledError):
                exceptions.append(exc)
            for t in self._tasks:
                t.cancel()
            tasks_cancelled = True
        try:
            if self._shutting_down:
                exceptions.extend(self._newly_finished_task_exceptions())
            else:
                self._shutting_down = True
            while self._tasks:
                self._finish_ping = self._loop.create_future()
                try:
                    await self._finish_ping
                except asyncio.CancelledError:
                    for sibling_task in self._tasks:
                        sibling_task.cancel()
                    raise
                new_exceptions = list(self._newly_finished_task_exceptions())
                exceptions.extend(new_exceptions)
                if not tasks_cancelled and new_exceptions:
                    for t in self._tasks:
                        t.cancel()
                    tasks_cancelled = True
        finally:
            self._loop = None
            self._shutting_down = False
        if reraise:
            # If mother task is cancelled, then propagate cancellation
            return False
        if exceptions:
            cls = (BaseExceptionGroup
                   if any(isinstance(e, BaseException) for e in exceptions)
                   else ExceptionGroup)
            raise cls('unhandled errors in a TaskGroup', tuple(exceptions))
    def _newly_finished_task_exceptions(self):
        for t in self._newly_finished_tasks:
            try:
                e = t.exception()
            except asyncio.CancelledError:
                continue
            if e:
                if isinstance(e, (KeyboardInterrupt, SystemExit)):
                    raise e
                yield e
        del self._newly_finished_tasks[:]

class TestTaskGroup(unittest.TestCase):
    def setUp(self):
        self.loop = EventLoop()

    def tearDown(self):
        self.loop.asyncio_loop.close()

    def test_task_group(self):
        quiesce = self.loop.quiesce

        futs = [self.loop.create_future() for _ in range(10)]
        async def fut_coro(fut):
            return await fut
        async def never():
            await self.loop.create_future()
        async def nothing(): pass

        # Basic operation: started tasks are awaited before exiting scope,
        # order is insignificant
        async def g1():
            async with TaskGroup() as tg:
                return [tg.create_task(fut_coro(f)) for f in futs]
        task = self.loop.create_task(g1())
        quiesce()
        shuffled = list(enumerate(futs))
        random.shuffle(shuffled)
        for (i, f) in shuffled:
            f.set_result(i)
            self.assertFalse(task.done())
            quiesce()
        self.assertEqual([t.result() for t in task.result()], list(range(10)))

        async def g2():
            tg = TaskGroup()
            n = nothing()
            with self.assertRaises(RuntimeError):
                # tasks cannot be created before aenter
                tg.create_task(n)
            await n
            async with tg:
                pass
            with self.assertRaises(RuntimeError):
                # context manager is not reusable
                async with tg:
                    pass
            n = nothing()
            with self.assertRaises(RuntimeError):
                # tasks cannot be created after aexit
                tg.create_task(n)
            await n
        self.loop.run_until_complete(g2())

        exc = Exception('hello')
        async def one_subtask_raises(futs):
            async with TaskGroup() as tg:
                for fut in futs[:-1]:
                    tg.create_task(fut_coro(fut))
                await fut_coro(futs[-1])
        for i in range(3):
            futs = [self.loop.create_future() for _ in range(3)]
            task = self.loop.create_task(one_subtask_raises(futs))
            quiesce()
            futs[i].set_exception(exc)
            quiesce()
            self.assertEqual(task.exception().exceptions, (exc,))

        async def one_subtask_raises_no_await(futs):
            async with TaskGroup() as tg:
                for fut in futs:
                    tg.create_task(fut_coro(fut))
        for i in range(3):
            futs = [self.loop.create_future() for _ in range(3)]
            task = self.loop.create_task(one_subtask_raises_no_await(futs))
            quiesce()
            futs[i].set_exception(exc)
            quiesce()
            self.assertEqual(task.exception().exceptions, (exc,))

        subtask = None
        cancelled = False
        async def parent_cancelled_after_fallthrough():
            nonlocal subtask
            nonlocal cancelled
            try:
                async with TaskGroup() as tg:
                    subtask = tg.create_task(never())
            except asyncio.CancelledError:
                cancelled = True
                raise
        task = self.loop.create_task(parent_cancelled_after_fallthrough())
        quiesce()
        task.cancel()
        assert not cancelled
        quiesce()
        self.assertTrue(cancelled)
        self.assertTrue(subtask.cancelled())

        async def body_raises_after_creating_task():
            async with TaskGroup() as tg:
                tg.create_task(never())
                raise exc
        task = self.loop.create_task(body_raises_after_creating_task())
        quiesce()
        self.assertEqual(task.exception().exceptions, (exc,))

        async def g4(futs, excs):
            async def coro(fut):
                try:
                    await fut
                finally:
                    raise excs[fut]
            async with TaskGroup() as tg:
                for fut in futs[:-1]:
                    tg.create_task(coro(fut))
                await coro(futs[-1])
        for i in range(4):
            futs = [self.loop.create_future() for _ in range(4)]
            excs = {f: Exception(i) for (i, f) in enumerate(futs)}
            task = self.loop.create_task(g4(futs, excs))
            quiesce()
            random.shuffle(futs)
            futs[i].set_exception(excs[futs[i]])
            quiesce()
            self.assertEqual(set(task.exception().exceptions),
                             {excs[f] for f in futs})

        calls = []
        async def g5(fut):
            async with TaskGroup() as tg:
                tg.create_task(fut_coro(fut))
                try:
                    await never()
                finally:
                    n = nothing()
                    # can't create new tasks while shutting down
                    try:
                        tg.create_task(n)
                    except Exception as e:
                        calls.append(e)
                    await n
        fut = self.loop.create_future()
        task = self.loop.create_task(g5(fut))
        quiesce()
        fut.set_exception(exc)
        quiesce()
        assert task.exception().exceptions == (exc,)
        self.assertEqual(len(calls), 1)
        self.assertTrue(isinstance(calls[0], RuntimeError))

        async def g6(inner, outer):
            async with TaskGroup() as tg:
                return (tg.create_task(fut_coro(inner)), await fut_coro(outer))
        (inner, outer) = (self.loop.create_future(), self.loop.create_future())
        task = self.loop.create_task(g6(inner, outer))
        quiesce()
        inner.cancel()
        outer.set_result(3)
        quiesce()
        assert task.done()
        (t, res) = task.result()
        self.assertEqual((t.cancelled(), res), (True, 3))

        inner_task = None
        async def g7(inner, outer):
            async with TaskGroup() as tg:
                nonlocal inner_task
                inner_task = tg.create_task(fut_coro(inner))
                await fut_coro(outer)
        (inner, outer) = (self.loop.create_future(), self.loop.create_future())
        task = self.loop.create_task(g7(inner, outer))
        quiesce()
        inner.set_result(3)
        outer.cancel()
        quiesce()
        self.assertTrue(task.cancelled())
        self.assertEqual(inner_task.result(), 3)

        # see https://github.com/python/cpython/issues/102572
        async def runner():
            fut = self.loop.create_future()
            async def coro():
                try:
                    fut.set_exception(KeyboardInterrupt())
                    await self.loop.create_future()
                except asyncio.CancelledError:
                    raise Exception()
            with self.assertRaises(KeyboardInterrupt):
                async with TaskGroup() as tg:
                    tg.create_task(coro())
                    await fut
        self.loop.run_until_complete(runner())

        # Cancel during aexit works
        calls = []
        async def coro(fut):
            try:
                await never()
            finally:
                calls.append(await fut)
        async def g8(fut):
            async with TaskGroup() as tg:
                tg.create_task(coro(fut))
                await never()
        fut = self.loop.create_future()
        task = self.loop.create_task(g8(fut))
        quiesce()
        task.cancel()
        quiesce()
        fut.set_result(4)
        quiesce()
        self.assertTrue(task.cancelled())
        self.assertEqual(calls, [4])
        del calls[:]
