SET(SRC_DIR "../../src")

ADD_EXECUTABLE("buf_extend" "buf_extend.c")
ADD_EXECUTABLE("buf_reduce" "buf_reduce.c")

TARGET_LINK_LIBRARIES("buf_extend" "tests")
TARGET_LINK_LIBRARIES("buf_reduce" "tests")

ADD_TEST("buf/buf_extend" "buf_extend")
ADD_TEST("buf/buf_reduce" "buf_reduce")
