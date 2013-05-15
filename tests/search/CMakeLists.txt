SET(SRC_DIR "../../src")

ADD_EXECUTABLE("search_parse_terms"  "search_parse_terms.c")
ADD_EXECUTABLE("search_free"         "search_free.c")
ADD_EXECUTABLE("search_match_substr" "search_match_substr.c")
ADD_EXECUTABLE("search_match_exact"  "search_match_exact.c")
ADD_EXECUTABLE("search_match_regex"  "search_match_regex.c")

TARGET_LINK_LIBRARIES("search_parse_terms"  "tests")
TARGET_LINK_LIBRARIES("search_free"         "tests")
TARGET_LINK_LIBRARIES("search_match_substr" "tests")
TARGET_LINK_LIBRARIES("search_match_exact"  "tests")
TARGET_LINK_LIBRARIES("search_match_regex"  "tests")

ADD_TEST("search/parse_terms"       "search_parse_terms")
ADD_TEST("search/free"              "search_free")
ADD_TEST("search/match_substr"      "search_match_substr")
ADD_TEST("search/match_exact"       "search_match_exact")
ADD_TEST("search/match_regex"       "search_match_regex")
