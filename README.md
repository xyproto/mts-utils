# tstools2

Fork of `tstools`, previously hosted at `code.google.com/p/tstools`.

This is a collection of utilities for working with MPEG data on 64-bit Linux.

# Building and installing

* Install [cxx](https://github.com/xyproto/cxx)

Build and install the desired utility:

* `cxx -C tsplay`
* `sudo install -Dm755 -t /usr/bin tsplay/tsplay`

These utilites are supported right now:

* `tsplay`
* `es2ts`
* `esdots`

Pull requests are welcome.

# Additional info

The emphasis is on relatively simple tools which concentrate on MPEG (H.264 and
H.262) data packaged according to H.222 (i.e., TS or PS), with a particular
interest in checking for conformance.

Transport Stream (TS) is typically used for distribution of cable and satellite
data. Program Stream (PS) is typically used to store data on DVDs.

The tools are focused on:

* Quick reporting of useful data (`tsinfo`, `stream_type`)
* Giving a quick overview of the entities in the stream (`esdots`, `psdots`)
`* Reporting on TS packets (`tsreport`) or ES units/frames/fields (`esreport`)
* Simple manipulation of stream data (`es2ts`, `esfilter`, `esreverse`, `esmerge`, `ts2es`)
* Streaming of data, possibly with introduced errors (`tsplay`).

# Running the unittests

    cd common
    cxx test

# License

* MPL 1.1

# Version

* 2.0.0 (WIP)

