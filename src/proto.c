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

/** return values:
  * -1 - invalid type
  *  0 - empty
  *  1 - valid
  *  2 - valid, operation expected for type
  */
#define CHECK_REQ(buf,token,req_type,ret) \
  else if (strncmp((buf), (token), strlen(token)) == 0) \
    { req->type = (req_type); return (ret); }
int
_check_type(ipc_req_t *req, char *buf, size_t buf_len)
{

  ASSERT(req != NULL && buf != NULL, MSG_M_NULLPTR);

  if (buf_len == 0)
    return 0;

  if (false) {}
  CHECK_REQ(buf, "FILE",   REQ_FILE,   2)
  CHECK_REQ(buf, "TAG",    REQ_TAG,    2)
  CHECK_REQ(buf, "DB",     REQ_DB,     2)
  CHECK_REQ(buf, "HELLO",  REQ_HELLO,  1)
  CHECK_REQ(buf, "BYE",    REQ_BYE,    1)
  CHECK_REQ(buf, "HELP",   REQ_HELP,   1)

  return -1;
}

/** return values:
  * -1 - invalid operation
  *  0 - empty operation
  *  1 - valid
  *  2 - valid, data expected for this operation
  */

#define CHECK_OP(buf,token,operation,data_type,ret) \
  else if (strncmp((buf), (token), strlen(token)) == 0) \
    { req->op = (operation); req->data.type = data_type; return (ret); }

int
_check_operation(ipc_req_t *req, char *buf, size_t buf_len)
{
  if (buf_len == 0)
    return 0;

  switch (req->type)
    {
      case REQ_FILE :
        if (false) {}
        CHECK_OP(buf, "ADD",     OP_F_ADD,     DATA_L_FILES, 2)
        CHECK_OP(buf, "DEL",     OP_F_DEL,     DATA_L_FILES, 2)
        CHECK_OP(buf, "SEARCH",  OP_F_SEARCH,  DATA_L_FILES, 2)
        break;
      case REQ_TAG :
        if (false) {}
        CHECK_OP(buf, "ADD",     OP_T_ADD,     DATA_M_UUID_TAGS, 2)
        CHECK_OP(buf, "DEL",     OP_T_DEL,     DATA_M_UUID_TAGS, 2)
        CHECK_OP(buf, "GET",     OP_T_GET,     DATA_L_UUIDS,     2)
        CHECK_OP(buf, "SET",     OP_T_SET,     DATA_M_UUID_TAGS, 2)
        CHECK_OP(buf, "CLR",     OP_T_CLR,     DATA_L_UUIDS,     2)
        break;
      case REQ_DB :
        if (false) {}
        CHECK_OP(buf, "STAT",    OP_D_STAT,    DATA_EMPTY, 1)
        CHECK_OP(buf, "CHECK",   OP_D_CHECK,   DATA_EMPTY, 1)
        break;
      default :
        break;
    }

  return -1;
}

/** return values:
  * -1 - invalid
  *  0 - empty
  *  1 - valid, parameters until "\n"
  *  2 - valid, parameters until "\n\n"
  */
int
_check_delimiter(ipc_req_t *req, char c)
{
  switch (c)
    {
      case '>' :
        req->data.flags |=  DATA_MULTI;
        return 2;
        break;
      case ':' :
        req->data.flags &= ~DATA_MULTI;
        return 1;
        break;
      case '\n' :
      case '\0' :
        return 0;
        break;
      default  :
        break;
    }

  return -1;
}

void
_rd_buf_extend(conn_t *conn, char *buf, size_t buf_len)
{
  char *t = NULL;
  size_t s_new = 0;

  s_new = conn->rd_buf_len + buf_len;

  t = realloc(conn->rd_buf, s_new + 1);
  ASSERT(t != NULL, MSG_M_REALLOC);
  conn->rd_buf = t;
  memcpy(&conn->rd_buf[conn->rd_buf_len], buf, buf_len);
  conn->rd_buf[s_new] = '\0';
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
ipc_request_read(conn_t *conn, ipc_req_t *req, char *buf, size_t buf_len)
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
      case  0 : /* empty request type */
      case -1 : /* invalid request type */
      default :
        data_item_add(&conn->errors, MSG_I_BADREQ, 0);
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
      case  0 : /* empty operation */
        data_item_add(&conn->errors, MSG_I_EXPOP, 0);
        while (isspace(*e)) e++;
        _rd_buf_reduce(conn, e - conn->rd_buf);
        return 2; /* malformed request */
        break;
      case -1 : /* invalid operation fo this request */
      default :
        data_item_add(&conn->errors, MSG_I_BADOP, 0);
        while (isspace(*e)) e++;
        _rd_buf_reduce(conn, e - conn->rd_buf);
        return 2; /* malformed request */
        break;
    }

  /* check & set delimiter */
  e++;
  for (s = e;  isblank(*s); s++); /* skip leading spaces */
  ret = _check_delimiter(req, *s);

  if (ret <= 0)
    {
      data_item_add(&conn->errors, MSG_I_BADMARKER, 0);
      e = strchr(s, '\n');
      while (isspace(*e)) e++;
      _rd_buf_reduce(conn, e - conn->rd_buf);
      return 2; /* malformed request */
    }

  /* ok, extract data, copy it to request and truncate rd_buf */
  for (s++; isblank(*s); s++); /* skip leading spaces */
  if (*s == '\n')
    { /* no data found */
      while (isspace(*s)) s++;
     _rd_buf_reduce(conn, s - conn->rd_buf);
      return 2;
    }

  if (ret == 1)
    {
      e = strchr(s, '\n');
      STRNDUP(req->data.buf, s, e - s);
      req->data.len = e - s;
      while (isspace(*e)) e++;
      _rd_buf_reduce(conn, e - conn->rd_buf);
      return 0; /* request ready for processing */
    }

  if (ret == 2)
    {
      if ((e = strstr(s, "\n\n")) == NULL)
        return 1; /* incomplete request */
      STRNDUP(req->data.buf, s, e - s);
      req->data.len = e - s;
      if (req->data.type != DATA_T_MSG)
        for (s = req->data.buf; *s != '\0'; s++)
          if (*s == '\n') *s = '\0';
      while (isspace(*e)) e++;
      _rd_buf_reduce(conn, e - conn->rd_buf);
      req->data.flags |= DATA_MULTI;
      return 0; /* request ready for processing */
    }

  return 2; /* something went wrong */
}

/** return values:
  */
/*
int
ipc_request_write(conn_t *conn, ipc_req_t *req)
*/

/** return values:
  * 0 - responce ready
  * 1 - incomplete responce, saved in buf
  * 2 - malformed responce
  */
/*
int
ipc_responce_read(conn_t *conn, ipc_resp_t *resp, char *buf, size_t buf_len)
*/

/** return values:
  */
int
ipc_responce_write(conn_t *conn, ipc_resp_t *resp)
{
  char *buf = NULL;
  size_t buf_len = 1024;
  char *t = NULL;
  int i = 1;
  int ret = 0;
  const char *status = NULL;
  const char *type = NULL;

  ASSERT(conn != NULL, MSG_M_NULLPTR);
  ASSERT(resp != NULL, MSG_M_NULLPTR);

  for (i = 0; i < resp->data.len; i++)
    if (resp->data.buf[i] == '\0')
      resp->data.buf[i] = '\n';

  switch (resp->status)
    {
      case STATUS_OK  : status = "OK";  break;
      case STATUS_ERR : status = "ERR"; break;
      default : return 1;               break;
    }

  switch (resp->data.type)
    {
      case DATA_EMPTY       : type = "";      break;
      case DATA_T_MSG       : type = "MSG";   break;
      case DATA_M_UUID_FILE : type = "FILES"; break;
      case DATA_M_UUID_TAGS : type = "TAGS";  break;
      case DATA_L_FILES     : /* we should responce */
      case DATA_L_UUIDS     : /* only with mappings */
      default               : return 1;        break;
    }

  again:
  FREE(buf);
  CALLOC(buf, buf_len, sizeof(char));

  if (resp->data.type != DATA_EMPTY)
    {
      ret = snprintf(buf, buf_len, "%s %s %s%s", \
                     status, type, \
                     (resp->data.items > 1) ? ">\n" : ": ",
                     resp->data.buf);
    }
  else ret = snprintf(buf, buf_len, "%s\n", status);

  if (ret > buf_len)
    {
      buf_len = ret + 1;
      goto again;
    }

  if (conn->wr_buf_len == 0)
    {
      conn->wr_buf = buf;
      conn->wr_buf_len = ret + 1;
    }
  else
    {
      t = realloc(conn->wr_buf, conn->wr_buf_len + ret);
      ASSERT(t != NULL, MSG_M_REALLOC);
      conn->wr_buf = t;
      memcpy(&conn->wr_buf[conn->wr_buf_len - 1], buf, ret);
      conn->wr_buf_len += ret;
    }

  return 0;
}
