SET(SRC_DIR "../../src")
SET(SRC_FILES
    "${SRC_DIR}/common.c"
    "${SRC_DIR}/log.c"
    "${SRC_DIR}/list.c"
    "${SRC_DIR}/uuid.c"
    )

ADD_EXECUTABLE("list_validate" ${SRC_FILES} "list_validate.c")
ADD_EXECUTABLE("list_item_add" ${SRC_FILES} "list_item_add.c")
ADD_EXECUTABLE("list_item_del" ${SRC_FILES} "list_item_del.c")
ADD_EXECUTABLE("list_items_walk" ${SRC_FILES} "list_items_walk.c")
ADD_EXECUTABLE("list_items_split" ${SRC_FILES} "list_items_split.c")
ADD_EXECUTABLE("list_items_merge" ${SRC_FILES} "list_items_merge.c")
ADD_EXECUTABLE("list_parse_tags" ${SRC_FILES} "list_parse_tags.c")
ADD_EXECUTABLE("list_item_search" ${SRC_FILES} "list_item_search.c")
ADD_EXECUTABLE("list_merge" ${SRC_FILES} "list_merge.c")
ADD_EXECUTABLE("list_copy" ${SRC_FILES} "list_copy.c")
ADD_EXECUTABLE("list_idx"  ${SRC_FILES} "list_idx.c")

ADD_TEST("list/validate"   "list_validate")
ADD_TEST("list/item_add"   "list_item_add")
ADD_TEST("list/item_del"   "list_item_del")
ADD_TEST("list/items_walk" "list_items_walk")
ADD_TEST("list/items_split" "list_items_split")
ADD_TEST("list/items_merge" "list_items_merge")
ADD_TEST("list/parse_tags" "list_parse_tags")
ADD_TEST("list/item_search" "list_item_search")
ADD_TEST("list/merge" "list_merge")
ADD_TEST("list/copy" "list_copy")
ADD_TEST("list/idx"  "list_idx")
