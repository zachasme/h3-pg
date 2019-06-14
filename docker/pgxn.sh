#!/bin/bash
set -e

BASEDIR=$(dirname $(realpath "$0"))
cd $BASEDIR

docker build -f base.Dockerfile -t h3-pg:base ..
docker build -f pgxn.Dockerfile -t h3-pg:pgxn ..

cd ..
docker run --rm h3-pg:pgxn