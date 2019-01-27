ARG VERSION=9.6
FROM mdillon/postgis:${VERSION}
ARG VERSION

# Install dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    ca-certificates \
    cmake \
    git \
    postgresql-server-dev-$VERSION

# hacky solution to non-root user in entrypoint.sh
RUN chmod -R a+w \
    /usr/share/postgresql/${VERSION} \
    /usr/lib/postgresql/${VERSION}/lib

# Set up makeinstall entrypoint
COPY ./entrypoint.sh /docker-entrypoint-initdb.d/999-docker-test.sh

# Set up directory structure including source files
COPY --chown=postgres:postgres . /tmp/h3-pg
WORKDIR /tmp/h3-pg

CMD ["postgres", "--version"]
