SET(SRC_DIR "../../src")

ADD_EXECUTABLE("get_dirhash"    "get_dirhash.c")
ADD_EXECUTABLE("normalize_path" "normalize_path.c")
ADD_EXECUTABLE("memmem"         "memmem.c")

TARGET_LINK_LIBRARIES("get_dirhash"     "tests")
TARGET_LINK_LIBRARIES("normalize_path"  "tests")
TARGET_LINK_LIBRARIES("memmem"          "tests")

ADD_TEST("common/get_dirhash"          "get_dirhash")
ADD_TEST("common/normalize_path"       "normalize_path")
ADD_TEST("common/memmem"               "memmem")
