\pset tuples_only on
-- res 0 index
\set res0index '\'8059fffffffffff\''
-- center hex
\set center '\'81583ffffffffff\''
-- 7 child hexes in res 0 index
\set solid 'ARRAY(SELECT h3_h3_to_children(:res0index, 1))'
-- 6 child hexes in rim of res 0 index
\set hollow 'array_remove(:solid, :center)'

--
-- TEST h3_polyfill and h3_h3_set_to_linked_geo
--

-- h3_polyfill is inverse of h3_h3_set_to_linked_geo for set without holes
SELECT array_agg(result) is null FROM (
    SELECT h3_polyfill(exterior, holes, 1) result FROM (
        SELECT exterior, holes FROM h3_h3_set_to_linked_geo(:solid)
    ) qq
    EXCEPT SELECT unnest(:solid) result
) q;

-- h3_polyfill is inverse of h3_h3_set_to_linked_geo for set with a hole
SELECT array_agg(result) is null FROM (
    SELECT h3_polyfill(exterior, holes, 1) result FROM (
        SELECT exterior, holes FROM h3_h3_set_to_linked_geo(:hollow)
    ) qq
    EXCEPT SELECT unnest(:hollow) result
) q;




