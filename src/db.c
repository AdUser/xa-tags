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
    }

  if (errno != 0)
    msg(msg_error, "%s\n", strerror(errno));

  if (opts.db.readonly == true)
    flags = SQLITE_OPEN_READONLY;

  if (sqlite3_open_v2(opts.db.path, &db_conn, flags, NULL) != SQLITE_OK)
    msg(msg_error, MSG_D_FAILOPEN, sqlite3_errmsg(db_conn));
}

void
db_close(void)
{
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
  
  str = dirname(path);
  len = strlen(str);
  new_uuid->dname = crc16(str, len);

  str = &path[len];
  len = strlen(str);
  new_uuid->fname = crc16(str, len);

  sqlite3_bind_int(stmt, 1, new_uuid->dname);
  sqlite3_bind_int(stmt, 2, new_uuid->fname);
  sqlite3_bind_text(stmt, 3, path, -1, SQLITE_STATIC);

  if (sqlite3_step(stmt) != SQLITE_OK)
    {
      msg(msg_warn, MSG_D_FAILEXEC, sqlite3_errmsg(db_conn));
      return 1;
    }

  new_uuid->id = (uint64_t) sqlite3_last_insert_rowid(db_conn);

  ASSERT(new_uuid->id != 0, "Successfull insert, but zero id returned.\n");
  
  sqlite3_finalize(stmt);

  return 0;
}

/*
int db_file_update(char *path, uuid_t *new_uuid);
int db_file_del(uuid_t uuid);
int db_file_search_path(char *str, data_t *results);
int db_file_search_tag(char *str, data_t *results);

int db_tag_set(uuid_t uuid, char *tags);
int db_tag_get(uuid_t uuid, char **tags);
int db_tag_find(char *str, data_t *results);

int db_commit(void);
*/
