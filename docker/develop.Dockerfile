ARG UBUNTU
FROM h3-pg:base-${UBUNTU}
ARG UBUNTU
ARG POSTGRESQL
ARG POSTGIS

# Hacky solution to non-root user in CMD
RUN chmod -R a+w \
    /usr/share/postgresql/${POSTGRESQL} \
    /usr/lib/postgresql/${POSTGRESQL}/lib

USER postgres

CMD service postgresql start && \
    psql -c "CREATE EXTENSION postgis;" && \
    make install && \
    make installcheck && \
    make clean
