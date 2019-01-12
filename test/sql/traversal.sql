\set hexagon '\'880326b88dfffff\''
\set origin '\'880326b887fffff\''
\set pentagon '\'831c00fffffffff\''

SELECT h3_k_ring(:hexagon);
SELECT h3_k_ring(h3_k_ring(:hexagon));
SELECT * FROM h3_k_ring_distances(:hexagon);

SELECT h3_k_ring_distances(:pentagon);

SELECT h3_k_ring(:hexagon);
SELECT h3_hex_range(:hexagon);
SELECT h3_k_ring(:hexagon, 2);
SELECT h3_hex_range(:hexagon, 2);
SELECT * FROM h3_hex_range_distances(:hexagon, 2);

SELECT h3_hex_range(:hexagon);
SELECT h3_hex_range('880326b8ebfffff');
SELECT h3_hex_ranges('{880326b88dfffff,880326b8ebfffff}'::h3index[]);

SELECT h3_hex_range(:hexagon, 2);
SELECT h3_hex_ring(:hexagon, 1);
SELECT h3_hex_ring(:hexagon, 2);

SELECT h3_h3_get_resolution(:hexagon);
SELECT h3_h3_get_resolution(h3_k_ring(:hexagon));
SELECT h3_h3_get_resolution(h3_hex_range(:hexagon));

SELECT h3_k_ring(:pentagon);
SELECT h3_k_ring(h3_k_ring(:pentagon));

SELECT h3_h3_get_resolution(:pentagon);
SELECT h3_h3_get_resolution(h3_k_ring(:pentagon));

SELECT h3_h3_indexes_are_neighbors(:hexagon, '880326b8ebfffff'), h3_h3_indexes_are_neighbors('880326b881fffff', '880326b8ebfffff');

SELECT h3_distance('880326b881fffff', '880326b885fffff');
SELECT h3_distance('880326b881fffff', h3_h3_to_parent('880326b885fffff'));

SELECT h3_line('841c023ffffffff', '841c025ffffffff');

SELECT h3_experimental_h3_to_local_ij(:origin, :hexagon);

SELECT :hexagon, h3_experimental_local_ij_to_h3(:origin, h3_experimental_h3_to_local_ij(:origin, :hexagon));