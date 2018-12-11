-- basecell around middle
\set idx_basecell '\'8059fffffffffff\''
-- 7 child hexes in basecell
\set idz_7hex 'ARRAY(SELECT h3_h3_to_children(:idx_basecell, 1))'
-- 6 child hexes in outer rim of basecell
\set idz_6hex 'ARRAY(SELECT id FROM (SELECT h3_h3_to_children(:idx_basecell, 1) id) q WHERE id != \'81583ffffffffff\')'

SELECT h3_h3_get_resolution(:idx_basecell);

-- BACK AND FORTH
SELECT :idz_7hex;
SELECT (h3_h3_set_to_linked_geo(:idz_7hex)).exterior, (h3_h3_set_to_linked_geo(:idz_7hex)).holes;
SELECT h3_polyfill(exterior, holes, 1) FROM 
(
 SELECT *  FROM h3_h3_set_to_linked_geo(:idz_7hex)
) q;

SELECT :idz_6hex;
SELECT (h3_h3_set_to_linked_geo(:idz_6hex)).exterior, (h3_h3_set_to_linked_geo(:idz_6hex)).holes;
SELECT h3_polyfill(exterior, holes, 1) FROM 
(
 SELECT *  FROM h3_h3_set_to_linked_geo(:idz_6hex)
) q;




