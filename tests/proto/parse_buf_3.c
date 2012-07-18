#include "../tests.h"

#define PATH1 "~/walls/sakura.jpg"

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

  buf = "FILE ADD : " PATH1 "\n";

  ret = parse_buf(conn, req, buf, strlen(buf));
  assert(ret == 0);
  assert(conn->rd_buf_len == 0);
  assert(req->type == REQ_FILE && req->op == OP_F_ADD);
  assert(req->delim == ':');
  assert(req->data_len == strlen(PATH1));
  assert(strcmp(req->data, PATH1) == 0);
  FREE(req);

  /* 2: missing space between marker and data */
  CALLOC(req, 1, sizeof(ipc_req_t));
  buf = "FILE ADD :" PATH1 "\n";

  ret = parse_buf(conn, req, buf, strlen(buf));
  assert(ret == 0);
  assert(conn->rd_buf_len == 0);
  assert(req->type == REQ_FILE && req->op == OP_F_ADD);
  assert(req->delim == ':');
  assert(req->data_len == strlen(PATH1));
  assert(strcmp(req->data, PATH1) == 0);
  FREE(req);

  /* 3: wrong delimiter */
  CALLOC(req, 1, sizeof(ipc_req_t));
  buf = "FILE ADD ; " PATH1 "\n";

  ret = parse_buf(conn, req, buf, strlen(buf));
  assert(ret == 2);
  assert(conn->rd_buf_len == 0);

  FREE(req);

  /* 4: empty data in request */
  CALLOC(req, 1, sizeof(ipc_req_t));
  buf = "FILE ADD : \n";

  ret = parse_buf(conn, req, buf, strlen(buf));
  assert(ret == 2);
  assert(conn->rd_buf_len == 0);

  FREE(req);

  return 0;
}
