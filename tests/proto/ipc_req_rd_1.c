#include "../tests.h"

int main()
{
  conn_t *conn = NULL;
  ipc_req_t *req = NULL;
  char *buf = NULL;
  int ret = 0;

  SIGCATCH_INIT

  CALLOC(conn, 1, sizeof(conn_t));

  /* simple operations */
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

  return 0;
}
