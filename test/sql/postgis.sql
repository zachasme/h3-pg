-- Variables for testing
\set resolution 10
\set hexagon '\'8a63a9a99047fff\''

SELECT h3_geo_to_h3(h3_h3_to_geo(:hexagon), :resolution);

SELECT h3_h3_to_geo_boundary(:hexagon);

SELECT ST_AsEWKT( h3_h3_to_geo_boundary_geometry(:hexagon));
