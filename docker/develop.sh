#!/bin/bash
set -e

BASEDIR=$(dirname $(realpath "$0"))
cd $BASEDIR

UBUNTU=xenial
POSTGRESQL=10
BUILDARGS="--build-arg UBUNTU=${UBUNTU} --build-arg POSTGRESQL=${POSTGRESQL}"

docker build ${BUILDARGS} -f base.Dockerfile -t h3-pg:base-${UBUNTU} ..
docker build ${BUILDARGS} -f develop.Dockerfile -t h3-pg:develop-${UBUNTU} ..

cd ..
chmod -R a+w src
docker run --rm -v "$PWD":/tmp/h3-pg h3-pg:develop-${UBUNTU}