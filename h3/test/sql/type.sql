\pset tuples_only on
\set string '\'801dfffffffffff\''
\set asbigint 576988517884755967
\set hexagon ':string::h3index'
\set pentagon '\'844c001ffffffff\'::h3index'

--
-- TEST operators
--
SELECT :hexagon = :hexagon;
SELECT NOT :hexagon = :pentagon;
SELECT NOT :hexagon <> :hexagon;
SELECT :hexagon <> :pentagon;
SELECT :pentagon <@ h3_cell_to_parent(:pentagon);
SELECT bool_and(:pentagon @> c) FROM (
    SELECT h3_cell_to_children(:pentagon) c
) q;

--
-- TEST bigint casting
--
SELECT :asbigint = :hexagon::bigint;

SELECT :hexagon = :asbigint::h3index;

--
-- TEST binary io
--
CREATE FUNCTION h3_test_binary_io() RETURNS boolean LANGUAGE PLPGSQL AS $$
    DECLARE
        result boolean;
    BEGIN
        CREATE TEMPORARY TABLE h3_test_binary_send (hex h3index PRIMARY KEY);
        CREATE TEMPORARY TABLE h3_test_binary_recv (hex h3index PRIMARY KEY);
        INSERT INTO h3_test_binary_send (hex) SELECT * from h3_get_res_0_cells();

        -- base temp dir on platform
        IF (SELECT setting = 'windows' FROM pg_catalog.pg_file_settings WHERE name = 'dynamic_shared_memory_type')
        THEN
            COPY h3_test_binary_send TO 'C:\Windows\Temp\h3_test_binary.bin' (FORMAT binary);
            COPY h3_test_binary_recv FROM 'C:\Windows\Temp\h3_test_binary.bin' (FORMAT binary);
        ELSE
            COPY h3_test_binary_send TO '/tmp/h3_test_binary.bin' (FORMAT binary);
            COPY h3_test_binary_recv FROM '/tmp/h3_test_binary.bin' (FORMAT binary);
        END IF;

        -- same data after re-import
        SELECT array_agg(hex) is null INTO result FROM (
            SELECT hex FROM h3_test_binary_send
            EXCEPT SELECT hex FROM h3_test_binary_recv
        ) q;

        RETURN result;
    END
$$;
SELECT h3_test_binary_io();