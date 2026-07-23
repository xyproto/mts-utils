#!/bin/sh
scriptdir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$scriptdir"
docker build --no-cache -t slay:ubuntu_26_10 . && docker run --rm --name slay_ubuntu slay:ubuntu_26_10
