#!/bin/bash
set -e

LIBH3_DIR=$1

if [ -z "$LIBH3_DIR" ]
then
    # libh3 dir not set, so we print the installed bindings
    cat test/expected/install.out \
        | grep -o '\bh3_\w*' \
        | sed -r 's/\bh3_//g' \
        | grep -v -x -F -f scripts/extra-functions \
        | sort | uniq
else
    # libh3 dir set, so we print the expected bindings
    cat ${LIBH3_DIR}/binding-functions \
        | sed -r 's/\bh3//g' \
        | sed -r 's/([A-Z]|[0-9]+)/_\L\1/g' | sed 's/^_//' \
        | sed -r 's/h_3/h3/g' \
        | sed -r 's/i_i_i/iii/g' \
        | sed -e 's/\(.*\)/\L\1/' \
        | grep -v -x -F -f scripts/excluded-functions \
        | sort | uniq
fi
