#!/bin/sh

set -e

if [ ! -f `pwd`/src/robotraconteur/tools/build_files/old_ubuntu_build/old_ubuntu_build.sh ]; then
    echo "This script must be run from the parent dir of the Robot Raconteur source tree"
    exit 1
fi

podman run -v `pwd`/src/:/work/src -w /work --rm ubuntu:xenial /work/src/robotraconteur/tools/build_files/old_ubuntu_build/old_ubuntu_build.sh
