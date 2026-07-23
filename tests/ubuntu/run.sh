#!/bin/sh
scriptdir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$scriptdir"
docker build --no-cache -t slay:ubuntu_24_04 . && docker run --rm --name slay_ubuntu slay:ubuntu_24_04
