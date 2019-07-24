#!/bin/bash
set -e

BASEDIR=$(dirname $(realpath "$0"))
cd $BASEDIR

UBUNTU=xenial

docker build --build-arg UBUNTU=${UBUNTU} -f base.Dockerfile -t h3-pg:base-${UBUNTU} ..
docker build --build-arg UBUNTU=${UBUNTU} -f build.Dockerfile -t h3-pg:build-${UBUNTU} ..

cd ..
docker run --rm -v "$PWD":/tmp/h3-pg h3-pg:build-${UBUNTU}