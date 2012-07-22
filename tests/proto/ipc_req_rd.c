#include "../tests.h"

int main()
{
  conn_t *conn = NULL;
  ipc_req_t *req = NULL;
  char *buf = NULL;
  char *test = NULL;
  int ret = 0;

  SIGCATCH_INIT

  CALLOC(conn, 1, sizeof(conn_t));

  /** simple operations */
  /* 1: fully valid request */
  CALLOC(req, 1, sizeof(ipc_req_t));
  buf = "HELLO\n";

  conn_buf_extend(conn, 'r', buf, strlen(buf));
  ret = ipc_request_read(conn, req);
  assert(ret == 0);
  assert(req->type == REQ_HELLO && req->op == OP_NONE);
  assert(conn->rd_buf == NULL && conn->rd_buf_len == 0);
  FREE(req);

  /* 2: empty request */
  CALLOC(req, 1, sizeof(ipc_req_t));
  buf = "\n  \t   HELLO";

  conn_buf_extend(conn, 'r', buf, strlen(buf));
  ret = ipc_request_read(conn, req);
  assert(ret == 2);

  /* 3: continue processing the same buffer */
  /* buf = "  \t   HELLO"; */
  ret = ipc_request_read(conn, req);
  assert(ret == 1);

  /* add terminating '\n' to input buffer  */
  /* now request should be processed completely */
  conn_buf_extend(conn, 'r', "\n", 1);
  ret = ipc_request_read(conn, req);
  assert(ret == 0);
  assert(req->type == REQ_HELLO && req->op == OP_NONE);
  assert(conn->rd_buf == NULL && conn->rd_buf_len == 0);
  FREE(req);

  /* 4: invalid request*/
  CALLOC(req, 1, sizeof(ipc_req_t));
  buf = "QUIT\n";

  conn_buf_extend(conn, 'r', buf, strlen(buf));
  ret = ipc_request_read(conn, req);
  assert(ret == 2);
  assert(conn->rd_buf == NULL && conn->rd_buf_len == 0);
  FREE(req);


  /** requests with operations */
  /* 1: fully valid request */
  CALLOC(req, 1, sizeof(ipc_req_t));
  buf = "DB STAT\n";

  conn_buf_extend(conn, 'r', buf, strlen(buf));
  ret = ipc_request_read(conn, req);
  assert(ret == 0);
  assert(conn->rd_buf_len == 0);
  assert(req->type == REQ_DB && req->op == OP_D_STAT);
  FREE(req);

  /* 2: incorrect operation */
  CALLOC(req, 1, sizeof(ipc_req_t));
  buf = "DB TEST\n";

  conn_buf_extend(conn, 'r', buf, strlen(buf));
  ret = ipc_request_read(conn, req);
  assert(ret == 2);
  assert(conn->rd_buf_len == 0);

  /* 3: valid operation without '\n' */
  buf = "DB \t\t\t CHECK";
  conn_buf_extend(conn, 'r', buf, strlen(buf));
  ret = ipc_request_read(conn, req);
  assert(ret == 1);

  conn_buf_extend(conn, 'r', "\n", 1);
  ret = ipc_request_read(conn, req);;
  assert(ret == 0);
  assert(conn->rd_buf_len == 0);
  assert(req->type == REQ_DB && req->op == OP_D_CHECK);
  FREE(req);

#define PATH1 "~/walls/sakura.jpg"

  /** requests with operations and data */
  /* 1: fully valid request */
  CALLOC(req, 1, sizeof(ipc_req_t));

  buf = "FILE ADD : " PATH1 "\n";

  conn_buf_extend(conn, 'r', buf, strlen(buf));
  ret = ipc_request_read(conn, req);
  assert(ret == 0);
  assert(conn->rd_buf_len == 0);
  assert(req->type == REQ_FILE && req->op == OP_F_ADD);
  assert((req->data.flags & DATA_MULTI) == 0);
  assert(req->data.len == strlen(PATH1));
  assert(strcmp(req->data.buf, PATH1) == 0);
  FREE(req);

  /* 2: missing space between marker and data */
  CALLOC(req, 1, sizeof(ipc_req_t));
  buf = "FILE ADD :" PATH1 "\n";

  conn_buf_extend(conn, 'r', buf, strlen(buf));
  ret = ipc_request_read(conn, req);
  assert(ret == 0);
  assert(conn->rd_buf_len == 0);
  assert(req->type == REQ_FILE && req->op == OP_F_ADD);
  assert((req->data.flags & DATA_MULTI) == 0);
  assert(req->data.len == strlen(PATH1));
  assert(strcmp(req->data.buf, PATH1) == 0);
  FREE(req);

  /* 3: wrong delimiter */
  CALLOC(req, 1, sizeof(ipc_req_t));
  buf = "FILE ADD ; " PATH1 "\n";

  conn_buf_extend(conn, 'r', buf, strlen(buf));
  ret = ipc_request_read(conn, req);
  assert(ret == 2);
  assert(conn->rd_buf_len == 0);

  FREE(req);

  /* 4: empty data in request */
  CALLOC(req, 1, sizeof(ipc_req_t));
  buf = "FILE ADD : \n";

  conn_buf_extend(conn, 'r', buf, strlen(buf));
  ret = ipc_request_read(conn, req);
  assert(ret == 2);
  assert(conn->rd_buf_len == 0);

  FREE(req);

  /** requests with operations and multiple data items */
  /* 1: fully valid request */
  CALLOC(req, 1, sizeof(ipc_req_t));

  test = "FILE ADD >"
#include "../samples/test_list_of_files.inc"

  STRNDUP(buf, test, 4096);

  conn_buf_extend(conn, 'r', buf, strlen(buf));
  ret = ipc_request_read(conn, req);
  assert(ret == 1);
  conn_buf_extend(conn, 'r', "\n\n", 2);
  ret = ipc_request_read(conn, req);
  assert(ret == 0);
  assert(req->type == REQ_FILE && req->op == OP_F_ADD);
  assert((req->data.flags & DATA_MULTI) == 1);
  FREE(req);

  return 0;
}
