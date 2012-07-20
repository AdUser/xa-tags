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

#define CHECK_OP(buf,token,operation,data_type,ret) \
  else if (strncmp((buf), (token), strlen(token)) == 0) \
    { req->op = (operation); req->data.type = data_type; return (ret); }

int
_check_operation(ipc_req_t *req, char *buf, size_t buf_len)
{
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
      case '>' :
        req->data.flags |=  DATA_MULTIROW;
        return 2;
        break;
      case ':' :
        req->data.flags &= ~DATA_MULTIROW;
        return 1;
        break;
      default  :
        break;
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
    {
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
      req->data.flags |= DATA_MULTIROW;
      return 0; /* request ready for processing */
    }

  return 2; /* something went wrong */
}

/** return values:
  * 0 - ok
  * 1 - error
  */
int
_validate_uuid(char *uuid)
{
  int i = 0;
  char c = '\0';

  ASSERT(uuid != NULL, MSG_M_NULLPTR);

  for (i = 0; i < 20; i++)
    {
      c = (isxdigit(uuid[i])) ? '0' : uuid[i];
      switch (c)
        {
          case '0' :
            if (i == 6 || i == 13) return 1;
            break;
          case '-' :
            if (i != 6 && i != 13) return 1;
            break;
          case 0x0 :
          default :
            return 1;
            break;
        }
    }

  return 0;
}

/** return values:
  * 0 - ok
  * 1 - error
  */
int
_validate_path(char *path)
{
  char *p = NULL;

  ASSERT(path != NULL, MSG_M_NULLPTR);
  p = path;
  while (isblank(*p)) p++;

  if (*p == '\0')
    return 1; /* empty string */

  if (*p != '/' && strncmp("~/", p, 2) != 0)
    return 1; /* allowed paths must begin with '/' or '~/' */

  return 0;
}

/** return values:
  * 0 - all clear
  * 1 - general error
  * 2 - unallowed symbol in tags
  */
int
_validate_tags(char *tags)
{
  char  *p = NULL;

  ASSERT(tags != NULL, MSG_M_NULLPTR);

  for (p = tags; *p != '\0'; p++)
    if (*p == '\"')
      return 2;

  if (p > tags) p--;

  while (isspace(*p) || p >= tags) p--;

  if (*p == '\\') return 2;

  return 0;
}

/** return values:
  * 0 - all clear
  * 1 - empty data set
  * 2 - error
  */
int
validate_data(data_t *data)
 {
  char *t = NULL;
  char *item = NULL;
  size_t item_len = 0;
  int skip_item = 0;
  size_t read = 0;

  ASSERT(data != NULL, MSG_M_NULLPTR);

  if (data->type == DATA_EMPTY)
    {
      FREE(data->buf);
      data->len = 0;
      return 0;
    }

  item = data->buf;
  while(read < data->len)
    {
      item_len = strlen(item) + 1;
      switch (data->type)
        {
          case DATA_T_MSG :
            break;
          case DATA_L_FILES :
            if (_validate_path(item)) skip_item = 1;
            break;
          case DATA_L_UUIDS :
            if (_validate_uuid(item)) skip_item = 1;
            break;
          case DATA_M_UUID_TAGS :
            if (_validate_uuid(item) ||
                _validate_tags(&item[21]) ||
                !isblank(item[20]))
                  skip_item = 1;
            break;
          case DATA_M_UUID_FILE :
            if (_validate_uuid(item) ||
                _validate_path(&item[21]) ||
                !isblank(item[20]))
                  skip_item = 1;
            break;
          default :
            skip_item = 1;
            break;
        }
      if (skip_item)
        {
          data->len -= item_len;
          memmove(item, item + item_len, data->len - read);
          t = realloc(data->buf, data->len);
          if (t == NULL)
            {
              if (data->len == 0)
                {
                  data->type = DATA_EMPTY;
                  return 1;
                }
              /* TODO */
            }
          data->buf = t;
          item = &data->buf[read]; /* reset pointer after realloc() */
          skip_item = 0;
          continue;
        }
      data->items += 1;
      read += item_len;
      item = &data->buf[read];
    }

  if (data->items < 2 && data->flags & DATA_MULTIROW)
    data->flags &= ~DATA_MULTIROW;

  return (data->items > 0) ? 0 : 1;
}
