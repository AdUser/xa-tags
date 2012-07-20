SET(SRC_DIR "../../src")
SET(SRC_FILES "${SRC_DIR}/proto.c" "${SRC_DIR}/log.c")

ADD_EXECUTABLE("parse_buf_1" ${SRC_FILES} "parse_buf_1.c")
ADD_TEST("proto/parse_buf_simple_req" "parse_buf_1")

ADD_EXECUTABLE("parse_buf_2" ${SRC_FILES} "parse_buf_2.c")
ADD_TEST("proto/parse_buf_req_with_op" "parse_buf_2")

ADD_EXECUTABLE("parse_buf_3" ${SRC_FILES} "parse_buf_3.c")
ADD_TEST("proto/parse_buf_req_with_data" "parse_buf_3")

ADD_EXECUTABLE("validate_data" ${SRC_FILES} "validate_data.c")
ADD_TEST("proto/validate_data" "validate_data")
