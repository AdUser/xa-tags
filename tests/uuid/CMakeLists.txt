SET(SRC_DIR "../../src")
SET(SRC_FILES
    "${SRC_DIR}/common.c"
    "${SRC_DIR}/log.c"
    "${SRC_DIR}/uuid.c"
    )

ADD_EXECUTABLE("uuid_generate"  ${SRC_FILES} "uuid_generate.c")

ADD_TEST("uuid/generate"       "uuid_generate")
