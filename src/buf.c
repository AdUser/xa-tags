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
buf_extend(buf_t *buf, char *str, size_t str_len)
{
  char *t = NULL;
  size_t s_new = 0;

  ASSERT(buf != NULL && str != NULL, MSG_M_NULLPTR);

  if (str_len == 0)
    str_len = strlen(str);

  s_new += buf->len + str_len;

  t = realloc(buf->buf, (s_new + 1) * sizeof(char));
  ASSERT(t != NULL, MSG_M_REALLOC);
  buf->buf = t;
  memcpy(&buf->buf[buf->len], str, str_len);
  buf->buf[s_new] = '\0';
  buf->len = s_new;
}

void
buf_reduce(buf_t *buf, size_t len)
{
  char *t = NULL;
  size_t s_del  = 0; /* [abcdefghij\nklmnopqrstuvwxyz\0] */
  size_t s_keep = 0; /* |<  s_del >||<    s_keep     >|  */

  s_del = (buf->len < len) ? buf->len : len ;
  s_keep = buf->len - s_del;

  memmove(buf->buf, &buf->buf[s_del], s_keep);

  t = realloc(buf->buf, (s_keep + 1) * sizeof(char));
  ASSERT(t != NULL, MSG_M_REALLOC);
  buf->buf = t;
  buf->buf[s_keep] = '\0';
  buf->len = s_keep;
}

