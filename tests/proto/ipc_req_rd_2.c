#include "../tests.h"

int main()
{
  conn_t *conn = NULL;
  ipc_req_t *req = NULL;
  char *buf = NULL;
  int ret = 0;

  SIGCATCH_INIT

  CALLOC(conn, 1, sizeof(conn_t));

  /* requests with operations */
  /* 1: fully valid request */
  CALLOC(req, 1, sizeof(ipc_req_t));
  buf = "DB STAT\n";

  ret = ipc_request_read(conn, req, buf, strlen(buf));
  assert(ret == 0);
  assert(conn->rd_buf_len == 0);
  assert(req->type == REQ_DB && req->op == OP_D_STAT);
  FREE(req);

  /* 2: incorrect operation */
  CALLOC(req, 1, sizeof(ipc_req_t));
  buf = "DB TEST\n";

  ret = ipc_request_read(conn, req, buf, strlen(buf));
  assert(ret == 2);
  assert(conn->rd_buf_len == 0);

  /* 3: valid operation without '\n' */
  buf = "DB \t\t\t CHECK";
  ret = ipc_request_read(conn, req, buf, strlen(buf));
  assert(ret == 1);

  ret = ipc_request_read(conn, req, "\n", 1);
  assert(ret == 0);
  assert(conn->rd_buf_len == 0);
  assert(req->type == REQ_DB && req->op == OP_D_CHECK);
  FREE(req);

  return 0;
}
