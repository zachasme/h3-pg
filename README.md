# PostgreSQL bindings for H3

This library provides PostgreSQL bindings for the [H3 Core Library](https://github.com/uber/h3). For API reference, please see the [H3 Documentation](https://uber.github.io/h3).

Developed in collaboration with [Scandinavian Highlands](http://www.scandinavian-highlands.com).

## Prerequisites

* PostgreSQL 9.6 or higher (*including server headers*). It might work with earlier versions, we have not tested earlier than 9.6.
* C compiler (e.g., gcc)
* GNU Make
* H3

## Quick Overview

If the prerequsites are met you can use the PGXN Client to download, build, and install, e.g.:

```
$ pgxn install h3
$ pgxn load -d mydb h3
$ psql -d mydb
=# SELECT h3_geo_to_h3(POINT('37.3615593,-122.0553238'), 5);
```

## Manual Installation

First, you must build [H3](https://uber.github.io/h3).

```
git clone https://github.com/uber/h3.git
cd h3
cmake -DCMAKE_C_FLAGS=-fPIC .
make
sudo make install
```

Then build h3-pg:

```
git clone ...
cd h3-pg
make
sudo make install
```

Run `psql` and load/use extension in database:

```
CREATE_EXTENSION h3;

SELECT h3_h3_to_children('880326b88dfffff', 9);
    h3_h3_to_children
-----------------
 890326b88c3ffff
 890326b88c7ffff
 890326b88cbffff
 890326b88cfffff
 890326b88d3ffff
 890326b88d7ffff
 890326b88dbffff
```

## Development

We provide a Dockerfile for development without installation of H3 and Postgres. The following requires that your system has `docker` installed.

First, build the docker image:

```
docker build -t h3-pg .
```

Then, build the extension and run the test suite:

```
docker run --rm h3-pg
```

Afterwards, to quickly build and test changes, run:

```
chmod -R 777 .
docker run --rm -it -v "$PWD":/tmp/h3-pg h3-pg
```

It will mount the code as a volume, and also mount the test output directory,
so output can be inspected. The chmod might be needed if you get permission
denied errors.

## Usage

Generally all functions have been renamed from camelCase to snake_case with an added `h3_` prefix. This means a few functions will have a double `h3_h3_` prefix, but we chose this for consistency. For example `h3ToChildren` becomes `h3_h3_to_children`.

See [API reference](docs/api.md)

## License

This project is released under the [Apache 2.0 License](LICENSE.md).
