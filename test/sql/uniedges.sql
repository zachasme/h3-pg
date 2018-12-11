\set hexagon '\'880326b885fffff\''
\set neighbor '\'880326b887fffff\''
\set pentagon '\'831c00fffffffff\''

\set uniedge 'h3_get_h3_unidirectional_edge(:hexagon, :neighbor)'

SELECT :uniedge;

SELECT h3_h3_indexes_are_neighbors(:hexagon, :neighbor), TRUE expected;
SELECT h3_h3_indexes_are_neighbors(:hexagon, :hexagon), FALSE expected;

SELECT h3_h3_unidirectional_edge_is_valid(:uniedge), TRUE expected;
SELECT h3_h3_unidirectional_edge_is_valid(:hexagon), FALSE expected;

SELECT
	h3_get_origin_h3_index_from_unidirectional_edge(:uniedge),
	:hexagon expected;

SELECT
	h3_get_destination_h3_index_from_unidirectional_edge(:uniedge),
	:neighbor expected;

SELECT
	h3_get_h3_indexes_from_unidirectional_edge(:uniedge),
	(:hexagon, :neighbor) expected;

SELECT array_length(array_agg(edge), 1), 6 expected FROM (
	SELECT h3_get_h3_unidirectional_edges_from_hexagon(:hexagon) edge
) q;
SELECT array_length(array_agg(edge), 1), 5 expected FROM (
	SELECT h3_get_h3_unidirectional_edges_from_hexagon(:pentagon) edge
) q;

SELECT h3_get_origin_h3_index_from_unidirectional_edge(h3_get_h3_unidirectional_edges_from_hexagon(:hexagon)),
	h3_get_destination_h3_index_from_unidirectional_edge(h3_get_h3_unidirectional_edges_from_hexagon(:hexagon));

SELECT h3_get_unidirectional_edge_boundary(:uniedge);