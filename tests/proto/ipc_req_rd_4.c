#include "../tests.h"

#define PATH1 "~/walls/sakura.jpg"

int main()
{
  conn_t *conn = NULL;
  ipc_req_t *req = NULL;
  char *buf = NULL;
  char *test = NULL;
  int ret = 0;

  SIGCATCH_INIT

  CALLOC(conn, 1, sizeof(conn_t));

  /* requests with multiline requests */
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
