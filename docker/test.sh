#!/bin/bash
set -e

BASEDIR=$(dirname $(realpath "$0"))
cd $BASEDIR
cd ..

for UBUNTU in bionic xenial
do
    echo "Testing on Ubuntu ${UBUNTU}"

    echo " - Building Docker images"
    for IMAGE in base build pgxn
    do
        echo -n "   * $IMAGE... "
        result=$(docker build \
            --build-arg UBUNTU=${UBUNTU} \
            -f docker/${IMAGE}.Dockerfile \
            -t h3-pg:${IMAGE}-${UBUNTU} \
            . 2> /dev/null)
        if [[ $? -eq 0 ]] ; then
            echo "success"
        else
            echo "fail:"
            echo "$result"
        fi
    done

    echo " - Testing Docker images"
    for IMAGE in build pgxn
    do
        echo -n "   * $IMAGE... "
        result=$(docker run --rm h3-pg:${IMAGE}-${UBUNTU} 2> /dev/null)
        if [[ $? -eq 0 ]] ; then
            echo "success"
        else
            echo "fail:"
            echo "$result"
        fi
    done
done