# mts-utils

This is a fork of `tstools` - a set of utilities for working with MPEG data on 64-bit Linux.

The goal of this fork is not portability and not being a drop-in replacement, but to make it possible to compile all the utilities with a modern C++ compiler, and make them easy to package on a modern Linux distro.

## Current status

It is working, but the tests are in the process of being upgraded to C++26 and the CI configuration is a work in progress.

Tested on Arch Linux with GCC 16.1.1.

## tstools

`tstools` (MPEG Transport Stream Utilities) were formerly hosted at [`code.google.com/p/tstools`](https://code.google.com/p/tstools).

## Requirements

* C++ compiler with support for `-std=c++26`.
* GNU `make`.
* GNU `parallel`.
* The [`slay`](https://github.com/xyproto/slay) executable (build tool written in Go).

## Building and installing

Build a utility:

* `slay -C tsplay`

Install the desired utility (use `sudo` if required):

* `install -Dm755 -t /usr/bin tsplay/tsplay`

Install the corresponding man page (use `sudo` if required):

* `install -Dm644 -t /usr/share/man/man1 docs/mdoc/tsplay.1`

Or build and install everything:

* `sudo make install`

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

## TODO

- [ ] Fix all warnings.
- [ ] Use `enum class` for all enums.
- [ ] Add more comprehensive tests, to ensure that everything works while porting to C++17.
- [ ] Restructure header files, make `*_fns.h` just `*_.h` and place the function bodies that are not `inline` in `.cpp` files.
- [ ] Remove `static` from functions that doesn't need it.
- [ ] Use `const std::string` whenever possible.
- [ ] Use `bool` whenever possible, instead of `int`.

## Additional info

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

## Running tests

    cd common
    slay test

(upgrading the tests is a work in progress)

## General info:

* License: MPL 1.1
* Version: 2.2.0
