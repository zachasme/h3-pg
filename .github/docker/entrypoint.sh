#!/bin/sh -l
set -e

CMD=$1

service postgresql start
chmod -R a+w h3/test

if [ -z "$CMD" ]; then
    make install
    su postgres -p -c "make ci"
    su postgres -p -c "make installcheck"
elif [ "$CMD" = "pgxn" ]; then
    DISTRIBUTION=$2
    su postgres -p -c "psql -c 'CREATE EXTENSION postgis'"
    pgxn install $DISTRIBUTION
    su postgres -p -c "pgxn load $DISTRIBUTION"
    su postgres -p -c "pgxn check $DISTRIBUTION"
elif [ "$CMD" = "pgxn-v3" ]; then
    DISTRIBUTION=$2
    su postgres -p -c "psql -c 'CREATE EXTENSION postgis'"
    pgxn install 'h3=3.7.2'
    su postgres -p -c "pgxn load 'h3=3.7.2'"
    su postgres -p -c "pgxn check 'h3=3.7.2'"
else
    ARGS="$@"
    make install
    su postgres -p -c "psql -c 'CREATE EXTENSION postgis'"
    su postgres -p -c "$ARGS"
fi
