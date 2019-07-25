#!/bin/bash
set -e

LIBH3_DIR=$1

diff <( \
    cat test/results/install.out \
        | grep -o '\bh3_\w*' \
        | sed -r 's/\bh3_//g' \
        | grep -v -x -F -f scripts/extra-functions \
        | sort | uniq \
) <( \
    cat ${LIBH3_DIR}/binding-functions \
        | sed -r 's/\bh3//g' \
        | sed -r 's/([A-Z]|[0-9]+)/_\L\1/g' | sed 's/^_//' \
        | sed -r 's/h_3/h3/g' \
        | sed -r 's/i_i_i/iii/g' \
        | sed -e 's/\(.*\)/\L\1/' \
        | grep -v -x -F -f scripts/excluded-functions \
        | sort | uniq \
)
