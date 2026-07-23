FROM ubuntu:latest

# Update and install packages
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y -q apt-file apt-utils build-essential git libc-dev make parallel golang && \
    apt-get update && \
    apt-get clean

# Install slay
RUN git clone https://github.com/xyproto/slay && \
    cd slay && \
    make && \
    make install

# Install and test mts-utils
RUN git clone https://github.com/xyproto/mts-utils && \
    cd mts-utils && \
    make && \
    make install && \
    make test
