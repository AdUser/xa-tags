SET(SRC_DIR "../../src")
SET(SRC_FILES
    "${SRC_DIR}/common.c"
    "${SRC_DIR}/log.c"
    "${SRC_DIR}/list.c"
    "${SRC_DIR}/uuid.c"
    "${SRC_DIR}/search.c"
    )

ADD_EXECUTABLE("search_parse_terms"  ${SRC_FILES} "search_parse_terms.c")
ADD_EXECUTABLE("search_free"         ${SRC_FILES} "search_free.c")
ADD_EXECUTABLE("search_match_substr" ${SRC_FILES} "search_match_substr.c")

ADD_TEST("search/parse_terms"       "search_parse_terms")
ADD_TEST("search/free"              "search_free")
ADD_TEST("search/match_substr"      "search_match_substr")

IF    (OPT_REGEX_SEARCH STREQUAL "ON")

ADD_EXECUTABLE("search_match_regex"  ${SRC_FILES} "search_match_regex.c")
ADD_TEST("search/match_regex"       "search_match_regex")

ENDIF (OPT_REGEX_SEARCH STREQUAL "ON")
