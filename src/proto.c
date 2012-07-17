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

ipc_resp_t *
make_ipc_resp(ipc_resp_t *resp, resp_status_t st, resp_data_t type, char *data)
{
  char *p = NULL;

  ASSERT(resp != NULL, MSG_M_NULLPTR);

  resp->status = st;
  resp->data_type = type;
  resp->data_len = strlen(data);

  if (resp->data_len <= 0)
    {
      resp->data_type = NO_DATA;
      return resp;
    }

  resp->delim = (strchr(data, '\n')) ? '>' : ':';

  CALLOC(resp->data, resp->data_len + 1, sizeof(char));
  memcpy(resp->data, data, resp->data_len);
  /* assert(data_len > 0) */
  p = &resp->data[resp->data_len - 1];
  if (*p != '\n')
    {
      *p = '\n';
      *(p + 1) = '\0';
    }

  return resp;
}

/** return values:
  * 0 - invalid type
  * 1 - valid
  * 2 - valid, operation expected for type
  */
#define CHECK_REQ(buf,token,req_type,ret) \
  else if (strncmp((buf), (token), strlen(token)) == 0) \
    { req->type = (req_type); return (ret); }
int
_check_type(ipc_req_t *req, char *buf, size_t buf_len)
{
  if (false) {}
  CHECK_REQ(buf, "FILE",   REQ_FILE,   2)
  CHECK_REQ(buf, "TAG",    REQ_TAG,    2)
  CHECK_REQ(buf, "DB",     REQ_DB,     2)
  CHECK_REQ(buf, "HELLO",  REQ_HELLO,  1)
  CHECK_REQ(buf, "BYE",    REQ_BYE,    1)
  CHECK_REQ(buf, "HELP",   REQ_HELP,   1)

  return 0;
}

/** return values:
  * 0 - invalid operation
  * 1 - valid
  * 2 - valid, data expected for this operation
  */

#define CHECK_OP(buf,token,operation,ret) \
  else if (strncmp((buf), (token), strlen(token)) == 0) \
    { req->op = (operation); return (ret); }

int
_check_operation(ipc_req_t *req, char *buf, size_t buf_len)
{
  switch (req->type)
    {
      case REQ_FILE :
        if (false) {}
        CHECK_OP(buf, "ADD",     OP_F_ADD,     2)
        CHECK_OP(buf, "DEL",     OP_F_DEL,     2)
        CHECK_OP(buf, "SEARCH",  OP_F_SEARCH,  2)
        break;
      case REQ_TAG :
        if (false) {}
        CHECK_OP(buf, "ADD",     OP_T_ADD,     2)
        CHECK_OP(buf, "DEL",     OP_T_DEL,     2)
        CHECK_OP(buf, "GET",     OP_T_GET,     2)
        CHECK_OP(buf, "SET",     OP_T_SET,     2)
        CHECK_OP(buf, "CLR",     OP_T_CLR,     2)
        break;
      case REQ_DB :
        if (false) {}
        CHECK_OP(buf, "STAT",    OP_D_STAT,    1)
        CHECK_OP(buf, "CHECK",   OP_D_CHECK,   1)
        break;
      default :
        break;
    }

  return 0;
}

/** return values:
  * 0 - invalid
  * 1 - valid, parameters until "\n"
  * 2 - valid, parameters until "\n\n"
  */
int
_check_delimiter(ipc_req_t *req, char c)
{
  switch (c)
    {
      case ':' : req->delim = ':';  return 1; break;
      case '>' : req->delim = '>';  return 2; break;
      default  : req->delim = '\0'; return 0; break;
    }

  return 0;
}

void
_rd_buf_extend(conn_t *conn, char *buf, size_t buf_len)
{
  char *t = NULL;
  size_t s_new = 0;

  s_new = conn->rd_buf_len + buf_len;

  t = realloc(conn->rd_buf, s_new + 1);
  ASSERT(t != NULL, MSG_M_REALLOC);
  memcpy(&conn->rd_buf[conn->rd_buf_len], buf, buf_len);
  conn->rd_buf[s_new + 1] = '\0';
  conn->rd_buf = t;
  conn->rd_buf_len = s_new;
}

void
_rd_buf_reduce(conn_t *conn, size_t len)
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

/** return values:
  * 0 - request ready for processing
  * 1 - incomplete request, saved in buf
  * 2 - malformed request
  */
int
parse_buf(conn_t *conn, ipc_req_t *req, char *buf, size_t buf_len)
{
  /* local variables */
  char *s = NULL; /* as 'start' */
  char *e = NULL; /* as 'end'   */
  int ret = 0;

  ASSERT(!((buf == NULL) && (buf_len != 0)), MSG_M_NULLPTR);

  if (conn->rd_buf_len != 0)
    { /* grow read buffer */
      _rd_buf_extend(conn, buf, buf_len);
    }
  else
    { /* save in read buffer */
      STRNDUP(conn->rd_buf, buf, buf_len);
      conn->rd_buf_len = buf_len;
    }

  if (memchr(conn->rd_buf, '\n', conn->rd_buf_len) == NULL)
    return 1; /* incomplete request */

  /* ok, we have at least one complete line in read buffer */
  /* so, check & set request type */
  s = conn->rd_buf;
  for (     ;  isblank(*s); s++); /* skip leading spaces */
  for (e = s;  isalpha(*e); e++);

  ret = _check_type(req, s, e - s);
  switch (ret)
    {
      case 2 : /* seems like we have request with operation */
        break;
      case 1 : /* short request types without parameters */
        while (isspace(*e)) e++;
        _rd_buf_reduce(conn, e - conn->rd_buf);
        return 0; /* request ready for processing */
        break;
      case 0 : /* invalid request type */
      default :
        msg(msg_warn, MSG_I_BADREQ);
        while (isspace(*e)) e++;
        _rd_buf_reduce(conn, e - conn->rd_buf);
        return 2; /* malformed request */
        break;
    }

  /* check & set operation */
  e++;
  for (s = e;  isblank(*s); s++); /* skip leading spaces */
  for (e = s;  isalpha(*e); e++);

  ret = _check_operation(req, s, e - s);
  switch (ret)
    {
      case 2 : /* sadly, but seems we have a lot of parameters in request */
        break;
      case 1 : /* short request types without parameters */
        while (isspace(*e)) e++;
        _rd_buf_reduce(conn, e - conn->rd_buf);
        return 0; /* request ready for processing */
        break;
      case 0 : /* invalid request type */
      default :
        msg(msg_warn, MSG_I_BADOP);
        while (isspace(*e)) e++;
        _rd_buf_reduce(conn, e - conn->rd_buf);
        return 2; /* malformed request */
        break;
    }

  /* check & set delimiter */
  e++;
  for (s = e;  isblank(*s); s++); /* skip leading spaces */
  ret = _check_delimiter(req, *s);

  if (ret == 0)
    return 2; /* malformed request */

  /* ok, extract data, copy it to request and truncate rd_buf */
  for (s++; isspace(*s); s++); /* skip leading spaces */

  if (ret == 1)
    {
      e = strchr(s, '\n');
      STRNDUP(req->data, s, e - s);
      while (isspace(*e)) e++;
      _rd_buf_reduce(conn, e - conn->rd_buf);
      return 0; /* request ready for processing */
    }

  if (ret == 2)
    {
      if ((e = strstr(s, "\n\n")) == NULL)
        return 1; /* incomplete request */
      STRNDUP(req->data, s, e - s);
      while (isspace(*e)) e++;
      _rd_buf_reduce(conn, e - conn->rd_buf);
      return 0; /* request ready for processing */
    }

  return 2; /* something went wrong */
}
