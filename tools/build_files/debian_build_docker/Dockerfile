# Dockerfile
ARG BASE_IMAGE=ubuntu:latest
FROM ${BASE_IMAGE} AS builder

ARG DEBIAN_FRONTEND=noninteractive
ARG CMAKE_EXTRA_ARGS=

ENV DEBIAN_FRONTEND=${DEBIAN_FRONTEND}
ENV CMAKE_EXTRA_ARGS=${CMAKE_EXTRA_ARGS}

WORKDIR /src

COPY . .

RUN apt update && apt install sudo tzdata python3 -y \
    && ln -fs /usr/share/zoneinfo/UTC /etc/localtime \
    && dpkg-reconfigure --frontend noninteractive tzdata

RUN apt install -y \
    zlib1g zlib1g-dev libssl-dev libusb-1.0-0 \
    libusb-1.0-0-dev libdbus-1-3 libdbus-1-dev libbluetooth3 libbluetooth-dev zlib1g zlib1g-dev \
    git cmake g++ make libboost-all-dev autoconf \
    automake libtool bison libpcre3-dev python3-dev python3-numpy python3-setuptools python3-wheel \
    python3-pytest curl libgtest-dev

RUN cmake -S . -B /build -DBUILD_GEN=ON -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=OFF $CMAKE_EXTRA_ARGS
RUN cmake --build /build  -- -j 4

RUN cd /build && tar cvf rr_out_debug.tar.gz out_debug test/out_debug
