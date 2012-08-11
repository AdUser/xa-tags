/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#include "common.h"

void
_handle_req_hello(conn_t *conn)
{
  ipc_resp_t *resp = NULL;
  char *buf = NULL;
  size_t buf_len = 1024;
  size_t len = 0;

  conn->flags |= CONN_F_HUMAN;

  CALLOC(resp, 1, sizeof(ipc_resp_t));

  do {
    FREE(buf);
    if (len > buf_len) buf_len = len + 1;
    CALLOC(buf, buf_len, sizeof(char));
    len = snprintf(buf, buf_len, MSG_I_HELLO,
                   PROGNAME, VERSION_MAJOR, VERSION_MINOR);
  } while (len > buf_len);

  resp->status = STATUS_OK;
  resp->data.type = DATA_T_MSG;
  resp->data.buf = buf;

  ipc_responce_write(conn, resp);

  FREE(buf);
  FREE(resp);
}

void
_handle_req_bye(conn_t *conn)
{
  ipc_resp_t *resp = NULL;
  char *buf = NULL;
  size_t buf_len = 1024;
  size_t len = 0;

  conn->flags |= CONN_F_CLOSE;

  CALLOC(resp, 1, sizeof(ipc_resp_t));

  do {
    FREE(buf);
    if (len > buf_len) buf_len = len + 1;
    CALLOC(buf, buf_len, sizeof(char));
    len = snprintf(buf, buf_len, MSG_I_BYE);
  } while (len > buf_len);

  resp->status = STATUS_OK;
  resp->data.type = DATA_T_MSG;
  data_item_add(&resp->data, buf, len);

  ipc_responce_write(conn, resp);

  FREE(buf);
  FREE(resp);
}

/** dispatcher */
void
handle_request(conn_t *conn, const ipc_req_t *req)
{
  ASSERT(req != NULL, MSG_M_NULLPTR);

  switch (req->type)
    {
      case REQ_HELLO :
        _handle_req_hello(conn);
        break;
      case REQ_BYE :
        _handle_req_bye(conn);
        break;
      case REQ_FILE :
        break;
      case REQ_TAG :
        break;
      case REQ_DB :
        break;
      default :
        break;
    }
}
