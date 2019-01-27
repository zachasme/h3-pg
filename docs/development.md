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

## Distribution on PGXN

Zip it up
```
git archive --format zip --prefix=h3-x.x.x/ --output h3-x.x.x.zip master
```

Upload it on https://manager.pgxn.org/