ARG UBUNTU
FROM h3-pg:base-${UBUNTU}
ARG UBUNTU
ARG POSTGRESQL
ARG POSTGIS

COPY . /tmp/h3-pg

RUN make install

USER postgres

CMD service postgresql start && \
    psql -c "CREATE EXTENSION postgis;" && \
    make installcheck
