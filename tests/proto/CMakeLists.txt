SET(SRC_DIR "../../src")
SET(SRC_FILES
    "${SRC_DIR}/proto.c"
    "${SRC_DIR}/buf.c"
    "${SRC_DIR}/log.c"
    "${SRC_DIR}/data.c"
    "${SRC_DIR}/uuid.c"
    )

ADD_EXECUTABLE("ipc_req_rd" ${SRC_FILES} "ipc_req_rd.c")
ADD_EXECUTABLE("ipc_resp_wr" ${SRC_FILES} "ipc_resp_wr.c")

ADD_TEST("proto/ipc_request_read" "ipc_req_rd")
ADD_TEST("proto/ipc_responce_write" "ipc_resp_wr")
