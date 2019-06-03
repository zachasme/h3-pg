\pset tuples_only on
\set degs 90.45
\set rads 1.57865030842887
\set epsilon 0.0000000000001

--
-- TEST h3_hex_area_km2
--

SELECT h3_hex_area_km2(10) = 0.0150475;

--
-- TEST h3_hex_area_m2
--

SELECT h3_hex_area_m2(10) = 15047.5;

--
-- TEST h3_edge_length_km
--

SELECT h3_edge_length_km(10) = 0.065907807;

--
-- TEST h3_edge_length_m
--

SELECT h3_edge_length_m(10) = 65.90780749;

--
-- TEST h3_num_hexagons
--

SELECT h3_num_hexagons(0) = 122;
SELECT h3_num_hexagons(15) = 569707381193162;

--
-- TEST h3_get_res_0_indexes
--

SELECT COUNT(*) = 122 FROM (SELECT h3_get_res_0_indexes()) q;