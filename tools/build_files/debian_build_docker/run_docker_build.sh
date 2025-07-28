#!/bin/bash

set -e

# Get absolute current script directory
BASEDIR=$(dirname "$(readlink -f "$0")")
echo "BASEDIR: $BASEDIR"

REPODIR=$(readlink -f "$BASEDIR/../../..")

cd $REPODIR

# Create random string for container name
CONTAINER_NAME="rr-build-$(date +%s | sha256sum |  head -c 8)"

echo "Running docker build with container name: $CONTAINER_NAME"

# Run docker build
docker build -t $CONTAINER_NAME -f $BASEDIR/Dockerfile $REPODIR $@

# Copy build artifacts
mkdir -p $REPODIR/docker_build
docker run -d --name $CONTAINER_NAME $CONTAINER_NAME /bin/bash
docker cp $CONTAINER_NAME:/build/rr_out_debug.tar.gz $REPODIR/docker_build/rr_out_debug.tar.gz
docker stop $CONTAINER_NAME
docker rm $CONTAINER_NAME

# docker rmi $CONTAINER_NAME
