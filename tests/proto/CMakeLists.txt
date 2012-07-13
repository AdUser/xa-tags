SET(SRC_DIR "../../src")
SET(SRC_FILES "${SRC_DIR}/proto.c" "${SRC_DIR}/log.c")

ADD_EXECUTABLE("parse_buf" ${SRC_FILES} "parse_buf.c")
ADD_TEST("proto/parse_buf" "parse_buf")
