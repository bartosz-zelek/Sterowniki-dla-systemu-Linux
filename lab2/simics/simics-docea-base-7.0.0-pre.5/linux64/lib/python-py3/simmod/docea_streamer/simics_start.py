from simmod.probe_monitor import simics_start as base_simics_start

streamer_classname = "docea-streamer"
streamer_confclassname = "docea_streamer"
streamer_obj_prefix = "ds"
streamer_header_doc = """The Docea probe-streamer is a tool extending on the
standard probe-streamer and dedicated to streaming out data produced by
the Docea simulator. It is mono-threaded in the sense that probes added to it
must run in the same thread as the streamer.
"""
streamer_create_cmd = "new-docea-streamer"
streamer_see_also = ["new-probe-streamer"]

sampler_argimpl = base_simics_start.SamplerArgImpl()
streamer_argimpl = base_simics_start.StreamerArgImpl(streamer_classname,
                                                     streamer_confclassname,
                                                     streamer_obj_prefix,
                                                     streamer_header_doc,
                                                     streamer_see_also)
streamer_argimpl_chain = base_simics_start.ArgImplChain(
    sampler_argimpl, streamer_argimpl)
streamer_argimpl_chain.create_cmd(streamer_create_cmd)
