SET(SRC_DIR "../../src")
SET(SRC_FILES
    "${SRC_DIR}/common.c"
    "${SRC_DIR}/log.c"
    )

ADD_EXECUTABLE("normalize_path" ${SRC_FILES} "normalize_path.c")

ADD_TEST("common/normalize_path"       "normalize_path")
