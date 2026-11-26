#!/bin/sh

# Build and test Robot Raconteur in a 32-bit x86 (i386) Debian sid container.
#
# Usage, from the parent dir of the source tree (see podman_sid_i386_build.sh):
#   sid_i386_build.sh build   install the build dependencies and build (run at full speed)
#   sid_i386_build.sh test    run the tests in an existing build (run on a slow, throttled CPU)
#
# The test stage is meant to run on a single, throttled core with the Java JIT disabled, to catch 32-bit bugs and
# tests that depend on timing (fixed sleeps, short timeouts).
# cSpell: ignore cpuset Xint nproc

set -e

CURRENT_DIR=$(pwd)
BUILD_DIR=$CURRENT_DIR/build_i386

case "$1" in
build)
    apt-get update
    apt-get install -y --no-install-recommends \
        default-jdk zlib1g-dev libssl-dev libusb-1.0-0-dev libdbus-1-dev libbluetooth-dev \
        git cmake g++ make ninja-build libboost-all-dev python3-dev python3-numpy python3-setuptools \
        python3-wheel python3-pytest libgtest-dev swig ca-certificates

    rm -rf "$BUILD_DIR"
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    cmake -G Ninja -DBUILD_GEN=ON -DBUILD_TESTING=ON -DBUILD_PYTHON3=ON -DBUILD_PYTHON3_WHEEL=ON \
        -DBUILD_JAVA=ON -DBUILD_NET=OFF -DBoost_USE_STATIC_LIBS=OFF -DCMAKE_BUILD_TYPE=Release \
        -DPYTHON3_EXECUTABLE=/usr/bin/python3 -DJAVA_HOME=/usr/lib/jvm/default-java \
        -DCMAKE_DISABLE_PRECOMPILE_HEADERS=ON \
        ../src/robotraconteur
    cmake --build . --config Release -- -j 4
    ;;
test)
    # Show the limits the tests run under: the CPUs and the CPU quota of the container, and whether the Java JIT is
    # disabled (JAVA_TOOL_OPTIONS=-Xint makes "java -version" report interpreted mode)
    echo "nproc: $(nproc)"
    echo "cpu.max: $(cat /sys/fs/cgroup/cpu.max 2>/dev/null || echo unknown)"
    java -version 2>&1 | tail -1
    cd "$BUILD_DIR"
    ctest . -C Release -E "robotraconteur_test_discovery_loopback|RobotRaconteurService.DiscoveryLoopback" \
        --timeout 1800 --output-on-failure
    ;;
*)
    echo "usage: $0 build|test"
    exit 1
    ;;
esac
