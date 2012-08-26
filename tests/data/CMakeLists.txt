SET(SRC_DIR "../../src")
SET(SRC_FILES
    "${SRC_DIR}/log.c"
    "${SRC_DIR}/data.c"
    )

ADD_EXECUTABLE("data_validate" ${SRC_FILES} "data_validate.c")
ADD_EXECUTABLE("data_item_add" ${SRC_FILES} "data_item_add.c")
ADD_EXECUTABLE("data_items_walk" ${SRC_FILES} "data_items_walk.c")
ADD_EXECUTABLE("data_items_split" ${SRC_FILES} "data_items_split.c")
ADD_EXECUTABLE("data_parse_tags" ${SRC_FILES} "data_parse_tags.c")
ADD_EXECUTABLE("data_item_search" ${SRC_FILES} "data_item_search.c")

ADD_TEST("data/validate"   "data_validate")
ADD_TEST("data/item_add"   "data_item_add")
ADD_TEST("data/items_walk" "data_items_walk")
ADD_TEST("data/items_split" "data_items_split")
ADD_TEST("data/parse_tags" "data_parse_tags")
ADD_TEST("data/item_search" "data_item_search")
