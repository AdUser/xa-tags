SET(SRC_DIR "../../src")
SET(SRC_FILES
    "${SRC_DIR}/log.c"
    "${SRC_DIR}/data.c"
    )

ADD_EXECUTABLE("data_validate" ${SRC_FILES} "data_validate.c")
ADD_TEST("data/data_validate" "data_validate")
