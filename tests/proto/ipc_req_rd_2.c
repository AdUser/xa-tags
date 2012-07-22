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

  return 0;
}
