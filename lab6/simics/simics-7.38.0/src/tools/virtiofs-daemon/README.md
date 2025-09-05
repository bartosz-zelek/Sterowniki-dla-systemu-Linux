# Virtiofs-daemon

This folder contains the source code for the virtiofs-daemon for Simics. The
`passthrough_hp_uds.cc` is a modified version of the
[`passthrough_hp.cc`](https://github.com/libfuse/libfuse/blob/fuse-3.13.0/example/passthrough_hp.cc)
example found in the libfuse upstream repository (version 3.13.0).

## Usage
The daemon is used by the fuse device, which starts the daemon and communicates
to it using unix domain sockets. The FUSE requests/responses are retrieved/sent
to the virtiofs device.
