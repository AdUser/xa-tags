SET(SRC_DIR "../../src")

ADD_EXECUTABLE("normalize_path" "normalize_path.c")
ADD_EXECUTABLE("memmem"         "memmem.c")

TARGET_LINK_LIBRARIES("normalize_path"  "tests")
TARGET_LINK_LIBRARIES("memmem"          "tests")

ADD_TEST("common/normalize_path"       "normalize_path")
ADD_TEST("common/memmem"               "memmem")
