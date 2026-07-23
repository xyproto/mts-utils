#!/bin/sh
docker build --no-cache -t slay:ubuntu_26_10 .
docker run --rm -it --name slay_ubuntu_interactive slay:ubuntu_26_10 bash
