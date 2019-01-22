# tstools2

Fork of `tstools`, previously hosted at `code.google.com/p/tstools`.

This is a set of command line tools for working with MPEG data on Linux.

# Building and installing

Only building `tsplay` on Linux is supported right now.

* Install [cxx](https://github.com/xyproto/cxx)
* `cd tsplay`
* `cxx sloppy`
* `sudo install -Dm755 tsplay /usr/bin/tsplay`

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

# License

* MPL 1.1

# Version

* 2.0.0 (WIP)

