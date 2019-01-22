# tstools2

Fork of `tstools`, formerly hosted at `code.google.com/p/tstools`.

This is a collection of utilities for working with MPEG data on 64-bit Linux.

# Requirements

* [cxx](https://github.com/xyproto/cxx)
* scons
* GNU `make`
* C++ compiler with support for C++2a or later (GCC 8.2.1 works).

# Building and installing

* Install [cxx](https://github.com/xyproto/cxx) (requires `scons`, GNU `make` and a C++ compiler with support for C++2a or later, like GCC 8.2.1).

Build and install the desired utility:

* `cxx -C tsplay`
* `sudo install -Dm755 -t /usr/bin tsplay/tsplay`

Install the corresponding man page:

*  `gzip docs/mdoc/tspla.1`
*  `sudo install -Dm644 -t /usr/share/man/man1 docs/mdoc/tsplay.1.gz`

The following utilites are available:

* `tsplay`
* `es_test`
* `esdots`
* `es2ts`
* `esfilter`
* `esmerge`
* `esreport`
* `esreverse`
* `m2ts2ts`
* `pcapreport`
* `psdots`
* `psreport`
* `rtp2264`
* `stream_type`
* `ps2ts`
* `ts2es`
* `ts2ps`
* `ts_packet_insert`
* `tsdvbsub`
* `tsfilter`
* `tsinfo`
* `tsreport`
* `tsserve`

# Additional info

The emphasis is on relatively simple tools which concentrate on MPEG (H.264 and
H.262) data packaged according to H.222 (i.e., TS or PS), with a particular
interest in checking for conformance.

Transport Stream (TS) is typically used for distribution of cable and satellite
data. Program Stream (PS) is typically used to store data on DVDs.

The tools are focused on:

* Quick reporting of useful data (`tsinfo`, `stream_type`)
* Giving a quick overview of the entities in the stream (`esdots`, `psdots`)
* Reporting on TS packets (`tsreport`) or ES units/frames/fields (`esreport`)
* Simple manipulation of stream data (`es2ts`, `esfilter`, `esreverse`, `esmerge`, `ts2es`)
* Streaming of data, possibly with introduced errors (`tsplay`).

# Running tests

    cd common
    cxx test

# License

* MPL 1.1

# Version

* 2.0.0 (WIP)

