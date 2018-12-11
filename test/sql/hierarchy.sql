\set hexagon '\'880326b88dfffff\''
\set pentagon '\'831c00fffffffff\''

SELECT array_length(array_agg(i), 1), 7 expected FROM (
	SELECT h3_h3_to_children(:hexagon) i
) q;

SELECT array_length(array_agg(i), 1), 6 expected FROM (
	SELECT h3_h3_to_children(:pentagon) i
) q;

SELECT h3_h3_to_children(:pentagon);

SELECT h3_h3_to_children(h3_h3_to_parent(:hexagon));
SELECT h3_h3_to_parent(h3_h3_to_children(:hexagon));

SELECT h3_h3_get_resolution(:hexagon);
SELECT h3_h3_get_resolution(h3_h3_to_parent(:hexagon));
SELECT h3_h3_get_resolution(h3_h3_to_children(:hexagon));

SELECT h3_h3_to_children(h3_h3_to_parent(:pentagon));
SELECT h3_h3_to_parent(h3_h3_to_children(:pentagon));

SELECT h3_h3_get_resolution(:pentagon);
SELECT h3_h3_get_resolution(h3_h3_to_parent(:pentagon));
SELECT h3_h3_get_resolution(h3_h3_to_children(:pentagon));

SELECT h3_compact(array_cat(ARRAY(SELECT h3_h3_to_children('880326b88dfffff')), ARRAY(SELECT h3_h3_to_children('880326b88bfffff'))));
--Checks that the uncompact call finds the sum of the two below calls. i.e. uncompacts all to same resolution
SELECT h3_uncompact(array_cat(ARRAY(SELECT h3_h3_to_children('880326b88dfffff')), '{880326b88bfffff}'::h3index[]));
SELECT h3_h3_to_children(h3_h3_to_children('880326b88bfffff'));
SELECT h3_h3_to_children(h3_h3_to_children('880326b88dfffff'));

SELECT h3_uncompact(array_cat(ARRAY(SELECT h3_h3_to_parent('880326b88dfffff')), '{880326b88bfffff}'::h3index[]));