-- PostGIS
CREATE OR REPLACE FUNCTION h3_geo_to_h3(geometry, resolution integer) RETURNS h3index
    AS $$ SELECT h3_geo_to_h3($1::point, $2); $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION h3_geo_to_h3(geography, resolution integer) RETURNS h3index
    AS $$ SELECT h3_geo_to_h3($1::geometry, $2); $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION h3_to_geometry(h3index) RETURNS geometry
  AS $$ SELECT ST_SetSRID(h3_to_geo($1)::geometry, 4326) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION h3_to_geography(h3index) RETURNS geography
  AS $$ SELECT h3_to_geometry($1)::geography $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION h3_to_geo_boundary_geometry(h3index, extend BOOLEAN default FALSE) RETURNS geometry
  AS $$ SELECT ST_SetSRID(h3_to_geo_boundary($1, $2)::geometry, 4326) $$ LANGUAGE SQL;
CREATE OR REPLACE FUNCTION h3_to_geo_boundary_geography(h3index, extend BOOLEAN default FALSE) RETURNS geography
  AS $$ SELECT h3_to_geo_boundary_geometry($1, $2)::geography $$ LANGUAGE SQL;

CREATE OR REPLACE FUNCTION h3_polyfill(multi geometry, resolution integer) RETURNS SETOF h3index
    AS $$ SELECT h3_polyfill(exterior, holes, resolution) FROM (
        SELECT 
            -- extract exterior ring of each polygon
            ST_MakePolygon(ST_ExteriorRing(poly))::polygon exterior,
            -- extract holes of each polygon
            (SELECT array_agg(hole)
                FROM (
                    SELECT ST_MakePolygon(ST_InteriorRingN(
                        poly,
                        generate_series(1, ST_NumInteriorRings(poly))
                    ))::polygon AS hole
                ) q_hole
            ) holes
        -- extract single polygons from multipolygon
        FROM (
            select (st_dump(multi)).geom as poly
        ) q_poly GROUP BY poly
    ) h3_polyfill; $$ LANGUAGE SQL IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION h3_polyfill(multi geography, resolution integer) RETURNS SETOF h3index
AS $$ SELECT h3_polyfill($1::geometry, $2) $$ LANGUAGE SQL;

-- Type casts
CREATE CAST (h3index AS point) WITH FUNCTION h3_to_geo(h3index);
CREATE CAST (h3index AS geometry) WITH FUNCTION h3_to_geometry(h3index);
CREATE CAST (h3index AS geography) WITH FUNCTION h3_to_geography(h3index);

