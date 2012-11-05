SET(SRC_DIR "../../src")
SET(SRC_FILES
    "${SRC_DIR}/common.c"
    "${SRC_DIR}/log.c"
    "${SRC_DIR}/uuid.c"
    )

ADD_EXECUTABLE("uuid_generate"     ${SRC_FILES} "uuid_generate.c")
ADD_EXECUTABLE("uuid_id_validate"  ${SRC_FILES} "uuid_id_validate.c")
ADD_EXECUTABLE("uuid_id_parse"     ${SRC_FILES} "uuid_id_parse.c")

ADD_TEST("uuid/generate"       "uuid_generate")
ADD_TEST("uuid/id_parse"       "uuid_id_parse")
ADD_TEST("uuid/id_validate"    "uuid_id_validate")
