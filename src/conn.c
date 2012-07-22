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
  size_t s_del  = 0; /* [abcdefghij\nklmnopqrstuvwxyz\0] */
  size_t s_keep = 0; /* |<  s_del >||<    s_keep     >|  */

  s_del = (conn->rd_buf_len < len) ? conn->rd_buf_len : len ;
  s_keep = conn->rd_buf_len - len;

  if (s_keep == 0)
    {
      FREE(conn->rd_buf);
      conn->rd_buf_len = 0;
      return;
    }

  t = memmove(conn->rd_buf, &conn->rd_buf[s_del], s_keep);
  ASSERT(t != NULL, MSG_M_NULLPTR);
  conn->rd_buf = realloc(t, s_keep + 1);
  ASSERT(conn->rd_buf != NULL, MSG_M_REALLOC);
  conn->rd_buf[s_keep] = '\0';
  conn->rd_buf_len = s_keep;
}
