.PHONY: all clean install install-man test

DESTDIR ?=
PREFIX ?= /usr

all:
	cxx -C es2ts opt
	cxx -C esdots opt
	cxx -C esfilter opt
	cxx -C esmerge opt
	cxx -C esreport opt
	cxx -C esreverse opt
	cxx -C m2ts2ts opt
	cxx -C pcapreport opt
	cxx -C ps2ts opt
	cxx -C psdots opt
	cxx -C psreport opt
	cxx -C rtp2264 opt
	cxx -C stream_type opt
	cxx -C ts2es opt
	cxx -C ts2ps opt
	cxx -C ts_packet_insert opt
	cxx -C tsdvbsub opt
	cxx -C tsfilter opt
	cxx -C tsinfo opt
	cxx -C tsplay opt
	cxx -C tsreport opt
	cxx -C tsserve opt

test:
	cxx -C common test

install: install-man
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

install-man:
	install -Dm644 -t "$(DESTDIR)$(PREFIX)/share/man/man1" docs/mdoc/es2ts.1
	install -Dm644 -t "$(DESTDIR)$(PREFIX)/share/man/man1" docs/mdoc/esdots.1
	install -Dm644 -t "$(DESTDIR)$(PREFIX)/share/man/man1" docs/mdoc/esfilter.1
	install -Dm644 -t "$(DESTDIR)$(PREFIX)/share/man/man1" docs/mdoc/esmerge.1
	install -Dm644 -t "$(DESTDIR)$(PREFIX)/share/man/man1" docs/mdoc/esreport.1
	install -Dm644 -t "$(DESTDIR)$(PREFIX)/share/man/man1" docs/mdoc/esreverse.1
	install -Dm644 -t "$(DESTDIR)$(PREFIX)/share/man/man1" docs/mdoc/m2ts2ts.1
	install -Dm644 -t "$(DESTDIR)$(PREFIX)/share/man/man1" docs/mdoc/pcapreport.1
	install -Dm644 -t "$(DESTDIR)$(PREFIX)/share/man/man1" docs/mdoc/ps2ts.1
	install -Dm644 -t "$(DESTDIR)$(PREFIX)/share/man/man1" docs/mdoc/psdots.1
	install -Dm644 -t "$(DESTDIR)$(PREFIX)/share/man/man1" docs/mdoc/psreport.1
	install -Dm644 -t "$(DESTDIR)$(PREFIX)/share/man/man1" docs/mdoc/rtp2264.1
	install -Dm644 -t "$(DESTDIR)$(PREFIX)/share/man/man1" docs/mdoc/stream_type.1
	install -Dm644 -t "$(DESTDIR)$(PREFIX)/share/man/man1" docs/mdoc/ts2es.1
	install -Dm644 -t "$(DESTDIR)$(PREFIX)/share/man/man1" docs/mdoc/ts_packet_insert.1
	install -Dm644 -t "$(DESTDIR)$(PREFIX)/share/man/man1" docs/mdoc/tsdvbsub.1
	install -Dm644 -t "$(DESTDIR)$(PREFIX)/share/man/man1" docs/mdoc/tsfilter.1
	install -Dm644 -t "$(DESTDIR)$(PREFIX)/share/man/man1" docs/mdoc/tsinfo.1
	install -Dm644 -t "$(DESTDIR)$(PREFIX)/share/man/man1" docs/mdoc/tsplay.1
	install -Dm644 -t "$(DESTDIR)$(PREFIX)/share/man/man1" docs/mdoc/tsreport.1
	install -Dm644 -t "$(DESTDIR)$(PREFIX)/share/man/man1" docs/mdoc/tsserve.1

clean:
	cxx -C es2ts clean
	cxx -C esdots clean
	cxx -C esfilter clean
	cxx -C esmerge clean
	cxx -C esreport clean
	cxx -C esreverse clean
	cxx -C m2ts2ts clean
	cxx -C pcapreport clean
	cxx -C ps2ts clean
	cxx -C psdots clean
	cxx -C psreport clean
	cxx -C rtp2264 clean
	cxx -C stream_type clean
	cxx -C ts2es clean
	cxx -C ts2ps clean
	cxx -C ts_packet_insert clean
	cxx -C tsdvbsub clean
	cxx -C tsfilter clean
	cxx -C tsinfo clean
	cxx -C tsplay clean
	cxx -C tsreport clean
	cxx -C tsserve clean
	cxx -C common clean
