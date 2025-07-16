#!/bin/sh

set -e

echo Running packaging test entry script

# detect debian or ubuntu
if [ -f /etc/debian_version ]; then
    # set default debian sources.list
    echo "deb http://deb.debian.org/debian sid main contrib non-free" > /etc/apt/sources.list
    # install python3 if not detected
    if [ ! -f /usr/bin/python3 ]; then
        apt-get update && apt-get install -y python3 sudo
    fi
fi

./packaging_test.py "$@"
