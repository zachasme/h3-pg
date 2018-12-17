\set geometry POINT('64.7498111652365,89.5695822308866')
\set resolution 8
\set hexagon '\'880326b88dfffff\''
\set pentagon '\'844c001ffffffff\''
\set pentagon_edgecross '\'831c00fffffffff\''

-- Index/coord conversions
SELECT h3_h3_to_geo(idx), h3_h3_get_resolution(idx) FROM (
    SELECT h3_geo_to_h3(:geometry, :resolution) AS idx
) AS q;
SELECT h3_geo_to_h3(geo, res) FROM (
    SELECT h3_h3_to_geo(:hexagon) AS geo, h3_h3_get_resolution(:hexagon) AS res
) AS q;
SELECT h3_geo_to_h3(geo, res) FROM (
    SELECT h3_h3_to_geo(:pentagon) AS geo, h3_h3_get_resolution(:pentagon) AS res
) AS q;

SELECT h3_h3_to_geo_boundary(:hexagon);
SELECT h3_h3_to_geo_boundary(:pentagon);
SELECT h3_h3_to_geo_boundary(:pentagon_edgecross);

SELECT h3_h3_to_geo('880326b88dfffff');
SELECT h3_geo_to_h3(POINT('64.7498111652365,89.5695822308866'), 8);

SELECT h3_h3_to_geo_boundary('80dbfffffffffff');
SELECT h3_h3_to_geo_boundary('80dbfffffffffff', true);