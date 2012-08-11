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

void
_handle_req_file_add(conn_t *conn, const ipc_req_t *req)
{
  ipc_resp_t resp;
  uuid_t new_uuid;
  char buf[MAXLINE];
  size_t len = 0;
  size_t buf_len = 0;
  uint16_t i = 0;
  char *p = NULL;

  ASSERT(conn != NULL && req != NULL, MSG_M_NULLPTR);

  memset(&resp, 0x0, sizeof(ipc_resp_t));
  resp.status = STATUS_ERR;

  p = req->data.buf;
  while (len < req->data.len)
    {
      memset(&new_uuid, 0x0, sizeof(uuid_t));
      p = &req->data.buf[len];
      len += strlen(p);
      if (db_file_add(p, &new_uuid) == 0)
        {
          buf_len = snprintf_m_uuid_file(buf, MAXLINE, &new_uuid, p);
          data_item_add(&resp.data, buf, buf_len);
          i++;
        }
    }

  if (resp.data.items > 0)
    {
      resp.status = STATUS_OK;
      resp.data.type = DATA_M_UUID_FILE;
    }

  if (i != req->data.items && i != 0)
    data_item_add(&conn->errors, MSG_I_PARTREQ, 0);

  ipc_responce_write(conn, &resp);

  data_clear(&resp.data);
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
        if (req->op == OP_F_ADD)
          _handle_req_file_add(conn, req);
        break;
      case REQ_TAG :
        break;
      case REQ_DB :
        break;
      default :
        break;
    }
}
