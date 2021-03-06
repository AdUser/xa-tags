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

/** hash & checksum functions */
/* -----------------8<------------------------ */
/* Copied from wikipedia */

/*
  Name  : CRC-16 CCITT
  Poly  : 0x1021
  Init  : 0xFFFF
  Revert: false
  XorOut: 0x0000
  Check : 0x29B1 ("123456789")
  MaxLen: 4095 bytes
*/

uint16_t
crc16(const char *str, size_t len)
{
  uint16_t crc = 0xFFFF;
  uint8_t i;

  while (len--)
    {
      crc ^= (*str++) << 8;

      for (i = 0; i < 8; i++)
          crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
    }

  return crc;
}

/*
  Name  : Jenkins Hash
*/

uint32_t
jhash(char *key, size_t len)
{
  uint32_t hash, i;

  for (hash = i = 0; i < len; ++i)
    {
      hash += key[i];
      hash += (hash << 10);
      hash ^= (hash >> 6);
    }

  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);

  return hash;
}
/* -----------------8<------------------------ */

/**
 @brief  calculates crc16 checksum for dirname() part of the provided path
 @param  path  path string to be processed
 @returns  checksum as 16bit unsigned integer
 */
uint16_t
get_dirhash(const char *path)
{
  const char *p = NULL;

  ASSERT(path != NULL, MSG_M_NULLPTR);

  if ((p = strrchr(path, '/')) == NULL)
    msg(msg_error, COMMON_ERR_FMTN, MSG_I_BADPATH, path);

  /* "/path/to/some/file" */
  /*  ^-- crc16 --^       */
  return crc16(path, p - path);
}

/** string functions */

/** returns allocated string with normalized path
 * 'path' must begin with either "/" or "~/"
 */
char *
normalize_path(const char *path)
{
  char buf[PATH_MAX] = { 0 };
  const char *s = NULL;
  const char *e = NULL;
  char *t = NULL;
  size_t len = 0;

  ASSERT(path != NULL, MSG_M_NULLPTR);
  memset(buf, 0x0, PATH_MAX);

  t = buf;
  s = path;
  if (*path == '/')
    s++;

  for ( ; (e = strchr(s, '/')) != NULL; s = e + 1)
    {
      len = e - s;
      switch (len)
        {
          case 2 :
            if (strncmp(s, "../", 3) == 0)
              {
                while (t > buf && *t != '/') t--;
                *t = '\0';
                continue;
              }
          case 1 : if (strncmp(s, "./",  2) == 0) continue;
          case 0 : if (strncmp(s, "/",   1) == 0) continue;
          default :
            if (*path != '~' || t != buf) *t++ = '/';
            memcpy(t, s, len * sizeof(char));
            t += len;
            break;
        }
    }

  if (t != buf) *t++ = '/';
  memcpy(t, s, strlen(s) * sizeof(char));

  STRNDUP(t, buf, PATH_MAX);

  return t;
}

#ifndef _GNU_SOURCE
/* strstr for memory */
void *
memmem(const void *haystack, size_t haystacklen,
       const void *needle,   size_t needlelen)
{
  size_t pos = 0;
  unsigned char *h = NULL;
  unsigned char *n = NULL;

  n = (unsigned char *) needle;
  for (pos = 0; pos <= haystacklen; pos++)
    {
      h = (unsigned char *) haystack;
      h += pos;
      if (*h == *n && (haystacklen - pos) >= needlelen && \
          memcmp(h, n, needlelen) == 0)
        return (void *) h;
    }

  return NULL;
}
#endif

void
mkdir_r(const char *path, mode_t mode)
{
  struct stat st;
  char *p = NULL;
  char *tmp = NULL;

  ASSERT(path != NULL, MSG_M_NULLPTR);

  STRNDUP(tmp, path, PATH_MAX);
  for (p = tmp + 1; (p = strchr(p, '/')) != NULL; p += 1)
    {
      *p = '\0';
      if (stat(tmp, &st) != 0)
        {
          if (errno != ENOENT)
            msg(msg_error, "%s\n", strerror(errno));
          if (mkdir(tmp, mode) != 0)
            msg(msg_error, "%s\n", strerror(errno));
        }
      *p = '/';
    }
  FREE(tmp);
}

/**
 @brief  custom ftw() with callback
 @param  path  top-level directory to start tree walk
 @param  str  arbitrary string
 @param  handler  user function to call on every found file/directory
                  path of file and 'str' will be passed to it as arguments.
 */
void
ftw_cb(const char *path, const char *str, void (*handler)(const char *, const char *))
{
  struct stat st;
  /* fts-related variables */
  const int fts_flags = FTS_PHYSICAL | FTS_NOCHDIR;
  char *fts_argv[2];
  FTS *fts = NULL;
  FTSENT *ftsent = NULL;

  stat(path, &st);

  if (S_ISREG(st.st_mode))
    {
      handler(path, str);
      return;
    }

  if (!S_ISDIR(st.st_mode))
    return;

  fts_argv[0] = (char * const) path;
  fts_argv[1] = NULL;

  if ((fts = fts_open(fts_argv, fts_flags, NULL)) == NULL)
    msg(msg_error, COMMON_ERR_FMTN, MSG_F_FAILOPEN, path);

  while ((ftsent = fts_read(fts)) != NULL)
    {
      if (file_xattr_supported(ftsent->fts_path) == 0)
        {
          msg(msg_warn, COMMON_ERR_FMTN, ftsent->fts_path, MSG_F_NOTSUPP);
          continue;
        }

      if (ftsent->fts_info & FTS_F)
        handler(ftsent->fts_path, str);

      /* note: we ignore postorder directories */
      if (ftsent->fts_info & FTS_D)
        handler(ftsent->fts_path, str);
    }

  fts_close(fts);
}

/** custom printf's */
size_t
snprintf_m_uuid_file(char *buf, size_t buf_len, const uuid_t *uuid, const char *path)
{
  size_t len = 0;

  len = snprintf(buf, buf_len, "%s %s", uuid_id_printf(uuid), path);

  return len;
}
/*
size_t snprintf_m_uuid_tags(char *buf, size_t buf_len, uuid_t *uuid, char *tags);
*/
