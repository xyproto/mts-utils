.PHONY: build clean install install-man test

DESTDIR ?=
PREFIX ?= /usr

build:
	CXXFLAGS='$(CXXFLAGS) -w' parallel cxx opt -C ::: es2ts esdots esfilter esmerge esreport esreverse m2ts2ts pcapreport ps2ts psdots psreport rtp2264 stream_type ts2es ts2ps ts_packet_insert tsdvbsub tsfilter tsinfo tsplay tsreport tsserve

test:
	cxx -C common test

install-man:
	parallel install -Dm644 -t "$(DESTDIR)$(PREFIX)/share/man/man1" ::: docs/mdoc/es2ts.1 docs/mdoc/esdots.1 docs/mdoc/esfilter.1 docs/mdoc/esmerge.1 docs/mdoc/esreport.1 docs/mdoc/esreverse.1 docs/mdoc/m2ts2ts.1 docs/mdoc/pcapreport.1 docs/mdoc/ps2ts.1 docs/mdoc/psdots.1 docs/mdoc/psreport.1 docs/mdoc/rtp2264.1 docs/mdoc/stream_type.1 docs/mdoc/ts2es.1 docs/mdoc/ts_packet_insert.1 docs/mdoc/tsdvbsub.1 docs/mdoc/tsfilter.1 docs/mdoc/tsinfo.1 docs/mdoc/tsplay.1 docs/mdoc/tsreport.1 docs/mdoc/tsserve.1

install: build install-man
	DESTDIR="$(DESTDIR)" PREFIX="$(PREFIX)" cxx -C es2ts install
	DESTDIR="$(DESTDIR)" PREFIX="$(PREFIX)" cxx -C esdots install
	DESTDIR="$(DESTDIR)" PREFIX="$(PREFIX)" cxx -C esfilter install
	DESTDIR="$(DESTDIR)" PREFIX="$(PREFIX)" cxx -C esmerge install
	DESTDIR="$(DESTDIR)" PREFIX="$(PREFIX)" cxx -C esreport install
	DESTDIR="$(DESTDIR)" PREFIX="$(PREFIX)" cxx -C esreverse install
	DESTDIR="$(DESTDIR)" PREFIX="$(PREFIX)" cxx -C m2ts2ts install
	DESTDIR="$(DESTDIR)" PREFIX="$(PREFIX)" cxx -C pcapreport install
	DESTDIR="$(DESTDIR)" PREFIX="$(PREFIX)" cxx -C ps2ts install
	DESTDIR="$(DESTDIR)" PREFIX="$(PREFIX)" cxx -C psdots install
	DESTDIR="$(DESTDIR)" PREFIX="$(PREFIX)" cxx -C psreport install
	DESTDIR="$(DESTDIR)" PREFIX="$(PREFIX)" cxx -C rtp2264 install
	DESTDIR="$(DESTDIR)" PREFIX="$(PREFIX)" cxx -C stream_type install
	DESTDIR="$(DESTDIR)" PREFIX="$(PREFIX)" cxx -C ts2es install
	DESTDIR="$(DESTDIR)" PREFIX="$(PREFIX)" cxx -C ts2ps install
	DESTDIR="$(DESTDIR)" PREFIX="$(PREFIX)" cxx -C ts_packet_insert install
	DESTDIR="$(DESTDIR)" PREFIX="$(PREFIX)" cxx -C tsdvbsub install
	DESTDIR="$(DESTDIR)" PREFIX="$(PREFIX)" cxx -C tsfilter install
	DESTDIR="$(DESTDIR)" PREFIX="$(PREFIX)" cxx -C tsinfo install
	DESTDIR="$(DESTDIR)" PREFIX="$(PREFIX)" cxx -C tsplay install
	DESTDIR="$(DESTDIR)" PREFIX="$(PREFIX)" cxx -C tsreport install
	DESTDIR="$(DESTDIR)" PREFIX="$(PREFIX)" cxx -C tsserve install

clean:
	parallel cxx clean -C ::: es2ts esdots esfilter esmerge esreport esreverse m2ts2ts pcapreport ps2ts psdots psreport rtp2264 stream_type ts2es ts2ps ts_packet_insert tsdvbsub tsfilter tsinfo tsplay tsreport tsserve common
