ARG UBUNTU=xenial
FROM h3-pg:base-${UBUNTU}
ARG UBUNTU
ARG POSTGRESQL
ARG POSTGIS
ARG PGXN=3.5.0

# Install dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    pgxnclient

RUN pgxnclient install h3=${PGXN}

USER postgres

CMD service postgresql start && \
    psql -c "CREATE EXTENSION postgis;" && \
    pgxnclient load h3  && \
    pgxnclient check h3
