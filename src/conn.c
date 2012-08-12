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

#include "handlers.h"

void
conn_on_errors(conn_t *conn)
{
  ipc_resp_t *resp = NULL;

  CALLOC(resp, 1, sizeof(ipc_resp_t));
  resp->status = STATUS_ERR;
  resp->data = conn->errors;
  ipc_responce_write(conn, resp);

  data_clear(&conn->errors);
  conn->errors.type = DATA_T_MSG;
  FREE(resp);
}

void
conn_on_read(conn_t *conn)
{
  int ret = 0;
  ipc_req_t *req = NULL;

  /* process read buffer up to either incomplete request or data end */
  while (1)
    {
      CALLOC(req, 1, sizeof(ipc_req_t));

      if ((ret = ipc_request_read(conn, req)) == 1)
        break; /* more data needed */

      if (ret == 2)
        {
          FREE(req);
          if (conn->errors.items)
            conn_on_errors(conn);
          continue;
        }

      ret = data_validate(&req->data, &conn->errors,
                          conn->flags & CONN_F_STRICT);
      if (ret > 0)
        {
          FREE(req);
          if (conn->errors.items)
            conn_on_errors(conn);
          continue;
        }

      if ((conn->flags & CONN_F_NOBATCH) && \
          (req->data.items > 1))
        {
          FREE(req);
          data_item_add(&conn->errors, MSG_I_NOBATCH, 0);
          conn_on_errors(conn);
          return;
        }

      if (req->data.items > 0 &&
          conn->errors.items > 0)
        {
          data_item_add(&conn->errors, MSG_I_PARTREQ, 0);
          conn_on_errors(conn);
          /* but continue */
        }

      handle_request(conn, req);

      FREE(req);
    }

  return;
}

void
conn_on_write(conn_t *conn)
{
  ssize_t len = 0;

  len = write(conn->fd, conn->wr.buf, conn->wr.len);

  if (len > 0)
    buf_reduce(&conn->wr, len);

  if (len < 0)
    msg(msg_warn, "%s", strerror(errno));

  return;
}
