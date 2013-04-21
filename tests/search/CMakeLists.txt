SET(SRC_DIR "../../src")
SET(SRC_FILES
    "${SRC_DIR}/common.c"
    "${SRC_DIR}/log.c"
    "${SRC_DIR}/data.c"
    "${SRC_DIR}/uuid.c"
    "${SRC_DIR}/search.c"
    )

ADD_EXECUTABLE("search_parse_terms"  ${SRC_FILES} "search_parse_terms.c")
ADD_EXECUTABLE("search_free"         ${SRC_FILES} "search_free.c")

ADD_TEST("search/parse_terms"       "search_parse_terms")
ADD_TEST("search/free"              "search_free")
