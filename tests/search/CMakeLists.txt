SET(SRC_DIR "../../src")
SET(SRC_FILES
    "${SRC_DIR}/common.c"
    "${SRC_DIR}/log.c"
    "${SRC_DIR}/data.c"
    "${SRC_DIR}/uuid.c"
    "${SRC_DIR}/search.c"
    )

ADD_EXECUTABLE("parse_search_terms"  ${SRC_FILES} "parse_search_terms.c")

ADD_TEST("search/parse_search_terms"       "parse_search_terms")
