#include "../tests.h"

int main()
{
  conn_t *conn = NULL;
  ipc_resp_t *resp = NULL;
  int ret = 0;

  SIGCATCH_INIT

  CALLOC(conn, 1, sizeof(conn_t));

  CALLOC(resp, 1, sizeof(ipc_resp_t));
  resp->status = STATUS_OK;
  resp->data.type = DATA_EMPTY;

  ret = ipc_responce_write(conn, resp);
  assert(ret == 0);
  assert(conn->wr_buf_len == 4);
  assert(memcmp(conn->wr_buf, "OK\n", 4) == 0);

  /*
  FREE(conn->wr_buf);
  conn->wr_buf_len = 0;
  */

  resp->status = STATUS_ERR;
  resp->data.type = DATA_EMPTY;

  ret = ipc_responce_write(conn, resp);
  assert(ret == 0);
  assert(conn->wr_buf_len == 8);
  assert(memcmp(conn->wr_buf, "OK\nERR\n", 8) == 0);

  return 0;
}
