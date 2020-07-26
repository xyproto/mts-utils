.NOTPARALLEL: build clean install install-man
.PHONY: build clean install install-man test

DESTDIR ?=
PREFIX ?= /usr

build:
	+CXXFLAGS='$(CXXFLAGS) -w' parallel cxx opt -C ::: es2ts esdots esfilter esmerge esreport esreverse m2ts2ts pcapreport ps2ts psdots psreport rtp2264 stream_type ts2es ts2ps ts_packet_insert tsdvbsub tsfilter tsinfo tsplay tsreport tsserve

test:
	+cxx -C common test

install: install-man
	+parallel install -Dm755 -t "$(DESTDIR)$(PREFIX)/bin" ::: es2ts/es2ts esdots/esdots esfilter/esfilter esmerge/esmerge esreport/esreport esreverse/esreverse m2ts2ts/m2ts2ts pcapreport/pcapreport ps2ts/ps2ts psdots/psdots psreport/psreport rtp2264/rtp2264 stream_type/stream_type ts2es/ts2es ts2ps/ts2ps ts_packet_insert/ts_packet_insert tsdvbsub/tsdvbsub tsfilter/tsfilter tsinfo/tsinfo tsplay/tsplay tsreport/tsreport tsserve/tsserve

install-man:
	+parallel install -Dm644 -t "$(DESTDIR)$(PREFIX)/share/man/man1" ::: docs/mdoc/es2ts.1 docs/mdoc/esdots.1 docs/mdoc/esfilter.1 docs/mdoc/esmerge.1 docs/mdoc/esreport.1 docs/mdoc/esreverse.1 docs/mdoc/m2ts2ts.1 docs/mdoc/pcapreport.1 docs/mdoc/ps2ts.1 docs/mdoc/psdots.1 docs/mdoc/psreport.1 docs/mdoc/rtp2264.1 docs/mdoc/stream_type.1 docs/mdoc/ts2es.1 docs/mdoc/ts_packet_insert.1 docs/mdoc/tsdvbsub.1 docs/mdoc/tsfilter.1 docs/mdoc/tsinfo.1 docs/mdoc/tsplay.1 docs/mdoc/tsreport.1 docs/mdoc/tsserve.1

clean:
	+parallel cxx clean -C ::: es2ts esdots esfilter esmerge esreport esreverse m2ts2ts pcapreport ps2ts psdots psreport rtp2264 stream_type ts2es ts2ps ts_packet_insert tsdvbsub tsfilter tsinfo tsplay tsreport tsserve common
