SET(SRC_DIR "../../src")
SET(SRC_FILES "${SRC_DIR}/proto.c" "${SRC_DIR}/log.c")

ADD_EXECUTABLE("parse_buf_1" ${SRC_FILES} "parse_buf_1.c")
ADD_TEST("proto/parse_buf_simple_req" "parse_buf_1")
