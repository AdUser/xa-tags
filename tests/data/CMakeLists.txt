SET(SRC_DIR "../../src")
SET(SRC_FILES
    "${SRC_DIR}/common.c"
    "${SRC_DIR}/log.c"
    "${SRC_DIR}/data.c"
    "${SRC_DIR}/uuid.c"
    )

ADD_EXECUTABLE("data_validate" ${SRC_FILES} "data_validate.c")
ADD_EXECUTABLE("data_item_add" ${SRC_FILES} "data_item_add.c")
ADD_EXECUTABLE("data_item_del" ${SRC_FILES} "data_item_del.c")
ADD_EXECUTABLE("data_items_walk" ${SRC_FILES} "data_items_walk.c")
ADD_EXECUTABLE("data_items_split" ${SRC_FILES} "data_items_split.c")
ADD_EXECUTABLE("data_items_merge" ${SRC_FILES} "data_items_merge.c")
ADD_EXECUTABLE("data_parse_tags" ${SRC_FILES} "data_parse_tags.c")
ADD_EXECUTABLE("data_item_search" ${SRC_FILES} "data_item_search.c")
ADD_EXECUTABLE("data_merge" ${SRC_FILES} "data_merge.c")

ADD_TEST("data/validate"   "data_validate")
ADD_TEST("data/item_add"   "data_item_add")
ADD_TEST("data/item_del"   "data_item_del")
ADD_TEST("data/items_walk" "data_items_walk")
ADD_TEST("data/items_split" "data_items_split")
ADD_TEST("data/items_merge" "data_items_merge")
ADD_TEST("data/parse_tags" "data_parse_tags")
ADD_TEST("data/item_search" "data_item_search")
ADD_TEST("data/merge" "data_merge")
