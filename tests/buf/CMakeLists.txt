SET(SRC_DIR "../../src")
SET(SRC_FILES
    "${SRC_DIR}/log.c"
    "${SRC_DIR}/buf.c"
    )

ADD_EXECUTABLE("buf_extend" ${SRC_FILES} "buf_extend.c")
ADD_EXECUTABLE("buf_reduce" ${SRC_FILES} "buf_reduce.c")

ADD_TEST("buf/buf_extend" "buf_extend")
ADD_TEST("buf/buf_reduce" "buf_reduce")
