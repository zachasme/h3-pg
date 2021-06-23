\pset tuples_only on

-- neighbouring indexes (one hexagon, one pentagon) at resolution 3
\set geo POINT(-144.52399108028, 49.7165031828995)
\set hexagon '\'831c02fffffffff\'::h3index'
\set pentagon '\'831c00fffffffff\'::h3index'
\set edgecross '\'8003fffffffffff\'::h3index'
\set resolution 3

--
-- TEST h3_cell_to_lat_lng and h3_lat_lng_to_cell
--

-- convertion to geo works
SELECT h3_cell_to_lat_lng(:hexagon) ~= :geo;

-- convertion to h3 index works
SELECT h3_lat_lng_to_cell(:geo, :resolution) = :hexagon;

-- h3_cell_to_lat_lng is inverse of h3_lat_lng_to_cell
SELECT h3_cell_to_lat_lng(i) ~= :geo AND h3_get_resolution(i) = :resolution FROM (
    SELECT h3_lat_lng_to_cell(:geo, :resolution) AS i
) AS q;
-- h3_lat_lng_to_cell is inverse of h3_cell_to_lat_lng
SELECT h3_lat_lng_to_cell(g, r) = :hexagon FROM (
    SELECT h3_cell_to_lat_lng(:hexagon) AS g, h3_get_resolution(:hexagon) AS r
) AS q;
-- same for pentagon
SELECT h3_lat_lng_to_cell(g, r) = :pentagon FROM (
    SELECT h3_cell_to_lat_lng(:pentagon) AS g, h3_get_resolution(:pentagon) AS r
) AS q;

--
-- TEST h3_cell_to_boundary
--

-- polyfill of geo boundary returns original index
SELECT h3_polygon_to_cells(h3_cell_to_boundary(:hexagon), null, :resolution) = :hexagon;

-- same for pentagon
SELECT h3_polygon_to_cells(h3_cell_to_boundary(:pentagon), null, :resolution) = :pentagon;

-- the boundary of an edgecrossing index is different with flag set to true
SELECT h3_cell_to_boundary(:hexagon) ~= h3_cell_to_boundary(:hexagon, true)
AND NOT h3_cell_to_boundary(:edgecross) ~= h3_cell_to_boundary(:edgecross, true);
