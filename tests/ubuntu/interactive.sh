#!/bin/sh
docker build --no-cache -t slay:ubuntu_24_04 .
docker run --rm -it --name slay_ubuntu_interactive slay:ubuntu_24_04 bash
