\set hexagon '\'831c02fffffffff\''
\set pentagon '\'831c00fffffffff\''
\set resolution 3

-- h3_h3_to_parent and h3_h3_to_children are inverses of each other for hexagons
SELECT h3_h3_to_children(h3_h3_to_parent(:hexagon)), :hexagon expected;
SELECT h3_h3_to_parent(h3_h3_to_children(:hexagon)), :hexagon expected;

-- h3_h3_to_parent and h3_h3_to_children are inverses of each other for pentagons
SELECT h3_h3_to_children(h3_h3_to_parent(:pentagon)), :pentagon expected;
SELECT h3_h3_to_parent(h3_h3_to_children(:pentagon)), :hexagon expected;

-- hexagon has 7 children
SELECT array_length(array_agg(hex), 1), 7 expected FROM (
	SELECT h3_h3_to_children(:hexagon) hex
) q;

-- pentagon has 6 children
SELECT array_length(array_agg(hex), 1), 6 expected FROM (
	SELECT h3_h3_to_children(:pentagon) hex
) q;

-- parent is one lower resolution
SELECT h3_h3_get_resolution(h3_h3_to_parent(:hexagon)), :resolution -1 expected;

-- children is one higher resolution
SELECT h3_h3_get_resolution(h3_h3_to_children(:hexagon)), :resolution +1 expected;


SELECT h3_compact(array_cat(ARRAY(SELECT h3_h3_to_children('880326b88dfffff')), ARRAY(SELECT h3_h3_to_children('880326b88bfffff'))));
--Checks that the uncompact call finds the sum of the two below calls. i.e. uncompacts all to same resolution
SELECT h3_uncompact(array_cat(ARRAY(SELECT h3_h3_to_children('880326b88dfffff')), '{880326b88bfffff}'::h3index[]));
SELECT h3_h3_to_children(h3_h3_to_children('880326b88bfffff'));
SELECT h3_h3_to_children(h3_h3_to_children('880326b88dfffff'));

SELECT h3_uncompact(array_cat(ARRAY(SELECT h3_h3_to_parent('880326b88dfffff')), '{880326b88bfffff}'::h3index[]));

-- h3_h3_to_children_slow and h3_h3_to_children have same result
SELECT h3_h3_to_children_slow(:hexagon), h3_h3_to_children(:hexagon) expected;