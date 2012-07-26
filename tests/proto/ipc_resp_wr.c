#include "../tests.h"

int main()
{
  conn_t *conn = NULL;
  ipc_resp_t *resp = NULL;
  int ret = 0;
  char *test = NULL;

  SIGCATCH_INIT

  CALLOC(conn, 1, sizeof(conn_t));

  /** simple responces */
  CALLOC(resp, 1, sizeof(ipc_resp_t));
  resp->status = STATUS_OK;
  resp->data.type = DATA_EMPTY;

  ret = ipc_responce_write(conn, resp);
  assert(ret == 0);
  assert(conn->wr.len == 3);
  assert(memcmp(conn->wr.buf, "OK\n", 4) == 0);

  resp->status = STATUS_ERR;
  resp->data.type = DATA_EMPTY;

  ret = ipc_responce_write(conn, resp);
  assert(ret == 0);
  assert(conn->wr.len == 7);
  assert(memcmp(conn->wr.buf, "OK\nERR\n", 8) == 0);

  FREE(conn->wr.buf);
  conn->wr.len = 0;

  /** oneline responces */
  memset(&resp->data, 0x0, sizeof(data_t));

  data_item_add(&resp->data, "Error message", 0);
  resp->status = STATUS_ERR;
  resp->data.type = DATA_T_MSG;

  ret = ipc_responce_write(conn, resp);
  assert(ret == 0);
  assert(conn->wr.len == 24);
  assert(memcmp(conn->wr.buf, "ERR MSG : Error message\n", 25) == 0);

  FREE(conn->wr.buf);
  conn->wr.len = 0;
  FREE(resp->data.buf);
  memset(&resp->data, 0x0, sizeof(data_t));

  /** multiline responces */
  data_item_add(&resp->data, "Error 1", 0);
  data_item_add(&resp->data, "Error 2", 0);
  data_item_add(&resp->data, "Error 3", 0);
  resp->status = STATUS_ERR;
  resp->data.type = DATA_T_MSG;

  test = \
  "ERR MSG >\n" \
  "Error 1\n" \
  "Error 2\n" \
  "Error 3\n" \
  "\n" ;
  ret = ipc_responce_write(conn, resp);
  assert(ret == 0);
  assert(conn->wr.len == 35);
  assert(memcmp(conn->wr.buf, test, 36) == 0);

  return 0;
}
