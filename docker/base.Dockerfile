ARG UBUNTU=xenial
FROM ubuntu:${UBUNTU}
ARG UBUNTU
ARG POSTGRESQL=10
ARG POSTGIS=2.4
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git

# xenial needs gnupg-curl for https support
RUN if [ "$UBUNTU" = "xenial" ] ; then apt-get install -y gnupg-curl; fi

# Setup PostgreSQL apt repository
RUN apt-key adv --fetch-keys https://www.postgresql.org/media/keys/ACCC4CF8.asc
RUN echo "deb http://apt.postgresql.org/pub/repos/apt/ ${UBUNTU}-pgdg main" >> /etc/apt/sources.list.d/pgdg.list
RUN apt-get update

# Install dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    postgresql-${POSTGRESQL}-postgis-${POSTGIS}-scripts \
    postgresql-${POSTGRESQL}-postgis-${POSTGIS} \
    postgresql-server-dev-${POSTGRESQL} \
    postgresql-${POSTGRESQL}

# Set workdir
WORKDIR /tmp/h3-pg