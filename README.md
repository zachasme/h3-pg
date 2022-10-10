# h3-pg: Uber's H3 Hexagonal Hierarchical Geospatial Indexing System in PostgreSQL

This library provides PostgreSQL bindings for the [H3 Core Library](https://github.com/uber/h3). For API reference, please see the [H3 Documentation](https://uber.github.io/h3).

Developed in collaboration with [Scandinavian Highlands](http://www.scandinavian-highlands.com).

## Prerequisites

- PostgreSQL 9.6+ (_including server headers_). It might work with earlier versions, we have not tested them.
- C compiler (e.g., gcc)
- GNU Make
- Git & CMake (for libh3)

## Quick Overview

If the prerequisites are met you can use the PGXN Client to download, build, and install, e.g.:

```shell
$ pgxn install h3
$ pgxn load h3
$ psql
=# SELECT h3_lat_lng_to_cell(POINT('37.3615593,-122.0553238'), 5);
  h3_lat_lng_to_cell
-----------------
 85e35e73fffffff
(1 row)
```

(You can install a specific version using `pgxn install 'h3=3.7.2'` and `pgxn load 'h3=3.7.2'` for example)

See [Advanced Install](#advanced-install) for other installation methods.

## Usage

> :tada: **Note:** The following usage docs apply to **H3 v4**, which was released on August 23, 2022.
>
> - For v3 docs, [see the latest v3.x.x release](https://github.com/zachasme/h3-pg/blob/v3.7.2/README.md).
> - For breaking changes in v4, [see the CHANGELOG](./CHANGELOG.md). In particular, most [function names have changed](https://h3geo.org/docs/library/migration-3.x/functions).

Generally, all functions have been renamed from camelCase in H3 to snake\_case in SQL with an added `h3_` prefix.

See [API reference](docs/api.md) for all provided functions.

## Advanced Install

### On macOS with the Postgres.app Universal Binary

```shell
brew install pgxnclient make cmake
export CMAKE_OSX_ARCHITECTURES="arm64;x86_64"
pgxn install h3
```

## Contributing

Pull requests and GitHub issues are welcome. Please include tests for new work. Please note that the purpose of this extension is to expose the API of the H3 Core library, so we will rarely accept new features that are not part of that API. New proposed feature work is more appropriate in the core C library or in a new extension that depends on h3-pg.

See [Development](docs/development.md).

## License

This project is released under the [Apache 2.0 License](LICENSE.md).
