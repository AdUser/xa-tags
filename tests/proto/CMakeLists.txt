SET(SRC_DIR "../../src")

ADD_EXECUTABLE("ipc_req_rd"  "ipc_req_rd.c")
ADD_EXECUTABLE("ipc_resp_wr" "ipc_resp_wr.c")

TARGET_LINK_LIBRARIES("ipc_req_rd"  "tests")
TARGET_LINK_LIBRARIES("ipc_resp_wr" "tests")

ADD_TEST("proto/ipc_request_read"   "ipc_req_rd")
ADD_TEST("proto/ipc_responce_write" "ipc_resp_wr")
