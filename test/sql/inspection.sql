\set invalid '\'0\''
\set hexagon '\'880326b88dfffff\''
\set pentagon '\'831c00fffffffff\''

SELECT h3_h3_is_pentagon(:hexagon), h3_h3_is_pentagon(:pentagon);

SELECT h3_h3_is_valid(:hexagon), h3_h3_is_valid(:pentagon), h3_h3_is_valid(:invalid);

SELECT h3_h3_get_resolution(:hexagon), h3_h3_get_resolution(:pentagon);

SELECT h3_h3_get_base_cell(:hexagon), h3_h3_get_base_cell(h3_h3_to_parent(:hexagon));
SELECT h3_h3_get_base_cell(:pentagon), h3_h3_get_base_cell(h3_h3_to_parent(:pentagon));

SELECT h3_h3_is_res_class_iii(:hexagon), h3_h3_is_res_class_iii(h3_h3_to_parent(:hexagon));
SELECT h3_h3_is_res_class_iii(:pentagon), h3_h3_is_res_class_iii(h3_h3_to_parent(:pentagon));