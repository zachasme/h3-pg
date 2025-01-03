\pset tuples_only on
\set hexagon '\'831c02fffffffff\'::h3index'

CREATE TABLE h3_test_spgist (hex h3index);
CREATE INDEX SPGIST_IDX ON h3_test_spgist USING spgist(hex);
INSERT INTO h3_test_spgist (hex) SELECT h3_cell_to_parent(:hexagon);
INSERT INTO h3_test_spgist (hex) SELECT h3_cell_to_children(:hexagon);
INSERT INTO h3_test_spgist (hex) SELECT h3_cell_to_center_child(:hexagon, 15);

--
-- TEST SP-GiST
--
SELECT COUNT(*) = 1 FROM h3_test_spgist WHERE hex @> :hexagon;
SELECT COUNT(*) = 8 FROM h3_test_spgist WHERE hex <@ :hexagon;

--

TRUNCATE TABLE h3_test_spgist;
INSERT INTO h3_test_spgist (hex) SELECT h3_cell_to_children(h3_cell_to_center_child(:hexagon, 10), 15);
SELECT COUNT(*) = 16807 FROM h3_test_spgist WHERE hex <@ :hexagon;
