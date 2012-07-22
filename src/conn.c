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
conn_buf_extend(conn_t *conn, char b, char *buf, size_t buf_len)
{
  char *t = NULL;
  size_t s_new = 0;
  char **sel = NULL;
  size_t *sel_len = NULL;

  ASSERT(conn != NULL && buf != NULL, MSG_M_NULLPTR);

  switch (b)
    {
      case 'r' : sel = &conn->rd_buf; sel_len = &conn->rd_buf_len; break;
      case 'w' : sel = &conn->wr_buf; sel_len = &conn->wr_buf_len; break;
      default  :
        assert(0);
        break;
    }

  s_new = *sel_len + buf_len;

  t = realloc(*sel, s_new + 1);
  ASSERT(t != NULL, MSG_M_REALLOC);
  *sel = t;
  memcpy(&(*sel)[*sel_len], buf, buf_len);
  (*sel)[s_new] = '\0';
  *sel_len = s_new;
}

void
conn_buf_reduce(conn_t *conn, char b, size_t len)
{
  char *t = NULL;
  char **sel = NULL;
  size_t *sel_len = NULL;
  size_t s_del  = 0; /* [abcdefghij\nklmnopqrstuvwxyz\0] */
  size_t s_keep = 0; /* |<  s_del >||<    s_keep     >|  */

  switch (b)
    {
      case 'r' : sel = &conn->rd_buf; sel_len = &conn->rd_buf_len; break;
      case 'w' : sel = &conn->wr_buf; sel_len = &conn->wr_buf_len; break;
      default  :
        assert(0);
        break;
    }


  s_del = (*sel_len < len) ? *sel_len : len ;
  s_keep = *sel_len - s_del;

  if (s_keep == 0)
    {
      FREE(*sel);
      *sel_len = 0;
      return;
    }

  memmove(*sel, &(*sel)[s_del], s_keep);
  t = realloc(*sel, s_keep + 1);
  ASSERT(t != NULL, MSG_M_REALLOC);
  *sel = t;
  (*sel)[s_keep] = '\0';
  *sel_len = s_keep;
}

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

  CALLOC(req, 1, sizeof(ipc_req_t));

  ret = ipc_request_read(conn, req);
  
  if (ret == 1)
    {
      FREE(req);
      return; /* more data needed */
    }
  
  if (ret == 2 && conn->errors.items > 0)
    goto abort_req;

  ret = data_validate(&req->data, &conn->errors,
                      conn->flags & CONN_F_STRICT);
  if (ret > 0)
    goto abort_req;

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

  /* TODO: request handle */

  return;

  abort_req: 
  FREE(req);
  data_item_add(&conn->errors, MSG_I_ABORTREQ, 0);
  conn_on_errors(conn);
  return;
}

void
conn_on_write(conn_t *conn)
{
  ;
}
