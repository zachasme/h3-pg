\set geometry POINT('64.7498111652365,89.5695822308866')
\set resolution 8
\set hexagon '\'880326b88dfffff\''
\set pentagon '\'844c001ffffffff\''
\set pentagon_edgecross '\'831c00fffffffff\''

-- Type conversions
SELECT h3_string_to_h3(h3_h3_to_string(:hexagon));
SELECT h3_h3_to_string(h3_string_to_h3(:hexagon));

SELECT h3_string_to_h3(:hexagon) = h3_string_to_h3(:hexagon);
SELECT h3_string_to_h3(:hexagon) = h3_string_to_h3(:pentagon);

SELECT h3_string_to_h3(:hexagon) <> h3_string_to_h3(:hexagon);
SELECT h3_string_to_h3(:hexagon) <> h3_string_to_h3(:pentagon);
