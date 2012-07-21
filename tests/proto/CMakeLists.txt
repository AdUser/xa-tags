SET(SRC_DIR "../../src")
SET(SRC_FILES
    "${SRC_DIR}/proto.c"
    "${SRC_DIR}/log.c"
    "${SRC_DIR}/data.c"
    )

ADD_EXECUTABLE("ipc_req_rd_1" ${SRC_FILES} "ipc_req_rd_1.c")
ADD_EXECUTABLE("ipc_req_rd_2" ${SRC_FILES} "ipc_req_rd_2.c")
ADD_EXECUTABLE("ipc_req_rd_3" ${SRC_FILES} "ipc_req_rd_3.c")
ADD_EXECUTABLE("ipc_req_rd_4" ${SRC_FILES} "ipc_req_rd_4.c")

ADD_TEST("proto/ipc_request_read_1" "ipc_req_rd_1")
ADD_TEST("proto/ipc_request_read_2" "ipc_req_rd_2")
ADD_TEST("proto/ipc_request_read_3" "ipc_req_rd_3")
ADD_TEST("proto/ipc_request_read_4" "ipc_req_rd_4")

ADD_EXECUTABLE("ipc_resp_wr_1" ${SRC_FILES} "ipc_resp_wr_1.c")

ADD_TEST("proto/ipc_responce_write_1" "ipc_resp_wr_1")
