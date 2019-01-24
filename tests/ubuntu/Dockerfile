FROM ubuntu:18.10

# Update and install packages
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y -q apt-utils build-essential git libc-dev make parallel python3 scons && \
    apt-get update && \
    apt-get clean

# Install cxx, but don't keep the git checkout in the docker image
RUN git clone https://github.com/xyproto/cxx && \
    cd cxx && \
    make install

# Install and test mts-utils
RUN git clone https://github.com/xyproto/mts-utils && \
    cd mts-utils && \
    make && \
    make install && \
    make test
