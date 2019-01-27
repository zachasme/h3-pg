# PostgreSQL bindings for H3

This library provides PostgreSQL bindings for the [H3 Core Library](https://github.com/uber/h3). For API reference, please see the [H3 Documentation](https://uber.github.io/h3).

Developed in collaboration with [Scandinavian Highlands](http://www.scandinavian-highlands.com).

## Prerequisites

* PostgreSQL 9.6 or higher (*including server headers*). It might work with earlier versions, we have not tested earlier than 9.6.
* C compiler (e.g., gcc)
* GNU Make
* Git & CMake (for libh3)

## Quick Overview

If the prerequsites are met you can use the PGXN Client to download, build, and install, e.g.:

```
$ pgxn install h3
$ pgxn load -d mydb h3
$ psql -d mydb
=# SELECT h3_geo_to_h3(POINT('37.3615593,-122.0553238'), 5);
  h3_geo_to_h3   
-----------------
 85e35e73fffffff
(1 row)
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

Generally, all functions have been renamed from camelCase in H3 to snake_case in SQL with an added `h3_` prefix (except when that would result in a double `h3_` prefix). For example `geoToH3` becomes `h3_geo_to_h3` and `h3ToChildren` becomes `h3_to_children`.

See [API reference](docs/api.md)

## License

This project is released under the [Apache 2.0 License](LICENSE.md).
