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

sqlite3 *db_conn = NULL;

void
db_open(void)
{
  int flags = SQLITE_OPEN_READWRITE;
  struct stat st;
  char *err = NULL;

  errno = 0;
  if (stat(opts.db.path, &st) == -1 && errno == ENOENT)
    {
      flags |= SQLITE_OPEN_CREATE;
      sqlite3_open_v2(opts.db.path, &db_conn, flags, NULL);
      if (sqlite3_exec(db_conn, SQL_DB_CREATE, NULL, NULL, &err) != SQLITE_OK)
        msg(msg_error, MSG_D_FAILCREATE, err);
      FREE(err);
      sqlite3_close(db_conn);
      errno = 0;
    }

  if (errno != 0)
    msg(msg_error, "%s\n", strerror(errno));

  if (opts.db.readonly == true)
    flags = SQLITE_OPEN_READONLY;

  if (sqlite3_open_v2(opts.db.path, &db_conn, flags, NULL) != SQLITE_OK)
    msg(msg_error, MSG_D_FAILOPEN, sqlite3_errmsg(db_conn));

#ifdef ASYNC_DB_WRITE
  sqlite3_exec(db_conn, "BEGIN  TRANSACTION;", NULL, NULL, NULL);
#endif
}

void
db_close(void)
{
#ifdef ASYNC_DB_WRITE
  sqlite3_exec(db_conn, "COMMIT TRANSACTION;", NULL, NULL, NULL);
#endif

  if (sqlite3_close(db_conn) != SQLITE_OK)
    msg(msg_error, MSG_D_FAILCLOSE, sqlite3_errmsg(db_conn));
}

int
db_file_add(char *path, uuid_t *new_uuid)
{
  sqlite3_stmt *stmt = NULL;
  char *str = NULL;
  size_t len = 0;

  len = strlen(SQL_F_ADD);
  if (sqlite3_prepare_v2(db_conn, SQL_F_ADD, len, &stmt, NULL) != SQLITE_OK)
    {
      msg(msg_warn, MSG_D_FAILPREPARE, sqlite3_errmsg(db_conn));
      return 1;
    }

  if ((str = strrchr(path, '/')) == NULL)
    return 1; /* path MUST contain at least one slash */

  len = str - path;
  new_uuid->dname = crc16(path, len);

  str += 1;
  len = strlen(str);
  new_uuid->fname = crc16(str, len);

  sqlite3_bind_int(stmt, 1, new_uuid->dname);
  sqlite3_bind_int(stmt, 2, new_uuid->fname);
  sqlite3_bind_text(stmt, 3, path, -1, SQLITE_STATIC);

  if (sqlite3_step(stmt) != SQLITE_DONE)
    {
      msg(msg_warn, MSG_D_FAILEXEC, sqlite3_errmsg(db_conn));
      return 1;
    }

  new_uuid->id = (uint64_t) sqlite3_last_insert_rowid(db_conn);

  ASSERT(new_uuid->id != 0, "Successfull insert, but zero id returned.\n");

  sqlite3_finalize(stmt);

  return 0;
}

int
db_file_update(char *path, uuid_t *new_uuid)
{
  sqlite3_stmt *stmt = NULL;
  char *str = NULL;
  size_t len = 0;

  len = strlen(SQL_F_UPDATE);
  if (sqlite3_prepare_v2(db_conn, SQL_F_UPDATE, len, &stmt, NULL) != SQLITE_OK)
    {
      msg(msg_warn, MSG_D_FAILPREPARE, sqlite3_errmsg(db_conn));
      return 1;
    }

  str = dirname(path);
  len = strlen(str);
  new_uuid->dname = crc16(str, len);

  str = &path[len];
  len = strlen(str);
  new_uuid->fname = crc16(str, len);

  sqlite3_bind_int(stmt, 1, new_uuid->dname);
  sqlite3_bind_int(stmt, 2, new_uuid->fname);
  sqlite3_bind_text(stmt, 3, path, -1, SQLITE_STATIC);
  sqlite3_bind_int64(stmt, 4, (sqlite3_int64) new_uuid->id);

  if (sqlite3_step(stmt) != SQLITE_OK)
    {
      msg(msg_warn, MSG_D_FAILEXEC, sqlite3_errmsg(db_conn));
      return 1;
    }

  sqlite3_finalize(stmt);

  return 0;
}

int
db_file_del(uuid_t uuid)
{
  sqlite3_stmt *stmt = NULL;
  size_t len = 0;

  len = strlen(SQL_F_DEL);
  if (sqlite3_prepare_v2(db_conn, SQL_F_DEL, len, &stmt, NULL) != SQLITE_OK)
    {
      msg(msg_warn, MSG_D_FAILPREPARE, sqlite3_errmsg(db_conn));
      return 1;
    }

  sqlite3_bind_int64(stmt, 1, (sqlite3_int64) uuid.id);

  if (sqlite3_step(stmt) != SQLITE_OK)
    {
      msg(msg_warn, MSG_D_FAILEXEC, sqlite3_errmsg(db_conn));
      return 1;
    }

  sqlite3_finalize(stmt);

  return 0;
}

int
db_file_search_path(char *str, data_t *results)
{
  sqlite3_stmt *stmt;
  uuid_t uuid = { 0, 0, 0 };
  size_t len = 0;
  int ret = 0;
  char buf[PATH_MAX + UUID_CHAR_LEN + 3] = { 0 };

  len = strlen(SQL_F_SEARCH);
  if (sqlite3_prepare_v2(db_conn, SQL_F_SEARCH, len, &stmt, NULL) != SQLITE_OK)
    {
      msg(msg_warn, MSG_D_FAILPREPARE, sqlite3_errmsg(db_conn));
      return 1;
    }

  snprintf(buf, PATH_MAX + 3, "%%%s%%", str);

  sqlite3_bind_text(stmt, 1, buf, -1, SQLITE_TRANSIENT);

  while ((ret = sqlite3_step(stmt)) == SQLITE_ROW)
    {
#ifdef UUID64
      uuid.id    = (uint64_t) sqlite3_column_int64(stmt, 0);
#else
      uuid.id    = (uint32_t) sqlite3_column_int64(stmt, 0);
#endif
      uuid.dname = (uint16_t) sqlite3_column_int(stmt, 1),
      uuid.fname = (uint16_t) sqlite3_column_int(stmt, 2),
      len = snprintf_m_uuid_file(buf, PATH_MAX + UUID_CHAR_LEN + 3,
                                 &uuid, (char *) sqlite3_column_text(stmt, 3));
      data_item_add(results, buf, len);
    }

  if (ret != SQLITE_DONE)
    {
      msg(msg_warn, MSG_D_FAILEXEC, sqlite3_errmsg(db_conn));
      data_clear(results);
      return 1;
    }

  if (results->items > 1)
    results->flags |= DATA_MULTI;
  results->type = DATA_M_UUID_FILE;

  sqlite3_finalize(stmt);

  return 0;
}

int
db_file_search_tag(char *str, data_t *results)
{
  sqlite3_stmt *stmt;
  size_t len = 0;
  int ret = 0;
  uuid_t uuid = { 0, 0, 0 };
  char buf[MAXLINE] = { 0 };

  len = strlen(SQL_T_SEARCH);
  if (sqlite3_prepare_v2(db_conn, SQL_T_SEARCH, len, &stmt, NULL) != SQLITE_OK)
    {
      msg(msg_warn, MSG_D_FAILPREPARE, sqlite3_errmsg(db_conn));
      return 1;
    }

  if (strchr(str, '*') != NULL)
    {
      snprintf(buf, MAXLINE, "%%%s%%", str);
      len = strlen(buf);
      while (len--)
        if (buf[len] == '*')
          buf[len] = '%';
    }
  else
    snprintf(buf, MAXLINE, "%% %s %%", str);

  sqlite3_bind_text(stmt, 1, buf, -1, SQLITE_TRANSIENT);

  while ((ret = sqlite3_step(stmt)) == SQLITE_ROW)
    {
      uuid.id    = (uint64_t) sqlite3_column_int64(stmt, 0);
      uuid.dname = (uint16_t) sqlite3_column_int(stmt, 1),
      uuid.fname = (uint16_t) sqlite3_column_int(stmt, 2),
      len = snprintf_m_uuid_file(buf, PATH_MAX + UUID_CHAR_LEN + 3,
                                 &uuid, (char *) sqlite3_column_text(stmt, 3));
      data_item_add(results, buf, len);
    }

  if (ret != SQLITE_DONE)
    {
      msg(msg_warn, MSG_D_FAILEXEC, sqlite3_errmsg(db_conn));
      data_clear(results);
      return 1;
    }

  if (results->items > 1)
    results->flags |= DATA_MULTI;
  results->type = DATA_M_UUID_FILE;

  sqlite3_finalize(stmt);

  return 0;
}

/*
int db_tag_set(uuid_t uuid, char *tags);
int db_tag_get(uuid_t uuid, char **tags);
int db_tag_find(char *str, data_t *results);

*/

void
db_commit(void)
{
#ifdef ASYNC_DB_WRITE
  sqlite3_exec(db_conn, "COMMIT TRANSACTION;", NULL, NULL, NULL);
  sqlite3_exec(db_conn, "BEGIN  TRANSACTION;", NULL, NULL, NULL);
#endif
}
