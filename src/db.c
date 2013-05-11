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

char *
db_find_path_user(void)
{
  char *b = NULL;
  char *p = NULL;

  CALLOC(b, PATH_MAX, sizeof(char));

  if      ((p = getenv("XDG_DATA_HOME")) != NULL);
  else if ((p = getenv("HOME")) != NULL);
  else    msg(msg_error, MSG_U_HOMEUNSET);

  snprintf(b, PATH_MAX, "%s/%s/%s", p, DB_USER_PATH, DB_FILENAME);

  STRNDUP(p, b, PATH_MAX);
  FREE(b);

  return p;
}

char *
db_find_path_system(void)
{
  char *b = NULL;
  char *p = NULL;

  CALLOC(b, PATH_MAX, sizeof(char));

  snprintf(b, PATH_MAX, "%s/%s", DB_SYSTEM_PATH, DB_FILENAME);

  STRNDUP(p, b, PATH_MAX);
  FREE(b);

  return p;
}

int
_db_check_version(void *unused, int columns, char **col_values, char **col_names)
{
  if (columns < 1)
    msg(msg_error, COMMON_MSG_FMTN, MSG_D_FAILOPEN, MSG_D_NOVERSION);

  if (col_values[0][0] != '1')
    msg(msg_error, COMMON_MSG_FMTN, MSG_D_VERMISMATCH, DB_VERSION);

  return 0;
}

char *
_db_get_fts_query(const search_t *search)
{
  char *query = NULL;
  size_t i = 0;

  if (search->substr.items != 0)
    {
      CALLOC(query, search->substr.len, sizeof(char));
      memcpy(query, search->substr.buf, search->substr.len);
      for (i = 0; i < search->substr.len - 1; i++)
        if (query[i] == '\0')
          query[i] = ' ';
    }
  else
    {
      STRNDUP(query, "*", 2);
    }

  return query;
}

void
db_open(void)
{
  int flags = SQLITE_OPEN_READWRITE;
  struct stat st;
  char *err = NULL;

  errno = 0;
  if (stat(opts.db.path, &st) == -1 && errno == ENOENT)
    {
#ifdef DB_AUTOCREATE
      mkdir_r(opts.db.path, 0755);
      flags |= SQLITE_OPEN_CREATE;
      if (sqlite3_open_v2(opts.db.path, &db_conn, flags, NULL) != SQLITE_OK)
        msg(msg_error, COMMON_ERR_FMTN, MSG_D_FAILOPEN, sqlite3_errmsg(db_conn));
      if (sqlite3_exec(db_conn, SQL_DB_CREATE_1, NULL, NULL, &err) != SQLITE_OK)
        msg(msg_error, COMMON_ERR_FMTN, MSG_D_FAILCREATE, err);
      if (sqlite3_exec(db_conn, SQL_DB_CREATE_2, NULL, NULL, &err) != SQLITE_OK)
        msg(msg_error, COMMON_ERR_FMTN, MSG_D_FAILCREATE, err);
      if (sqlite3_exec(db_conn, SQL_DB_CREATE_3, NULL, NULL, &err) != SQLITE_OK)
        msg(msg_error, COMMON_ERR_FMTN, MSG_D_FAILCREATE, err);
#ifdef UNIQ_TAGS_LIST
      if (sqlite3_exec(db_conn, SQL_DB_CREATE_UNIQ,   NULL, NULL, &err) != SQLITE_OK)
        msg(msg_error, COMMON_ERR_FMTN, MSG_D_FAILCREATE, err);
#endif
      if (sqlite3_exec(db_conn, SQL_DB_INIT,   NULL, NULL, &err) != SQLITE_OK)
        msg(msg_error, COMMON_ERR_FMTN, MSG_D_FAILCREATE, err);
      FREE(err);
      sqlite3_close(db_conn);
      errno = 0;
      msg(msg_warn, COMMON_MSG_FMTN, MSG_D_DBCREATED, opts.db.path);
#else
      err = strerror(errno);
      msg(msg_error, COMMON_ERR_FMTN, err, opts.db.path);
#endif
    }

  if (errno != 0)
    msg(msg_error, COMMON_ERR_FMTN, MSG_U_UNKNERR, strerror(errno));

  if (opts.db.readonly == true)
    flags = SQLITE_OPEN_READONLY;

  if (sqlite3_open_v2(opts.db.path, &db_conn, flags, NULL) != SQLITE_OK)
    msg(msg_error, COMMON_ERR_FMTN, MSG_D_FAILOPEN, sqlite3_errmsg(db_conn));

  sqlite3_exec(db_conn, SQL_DB_CHECKVERSION, _db_check_version, NULL, NULL);

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
    msg(msg_error, COMMON_ERR_FMTN, MSG_D_FAILCLOSE, sqlite3_errmsg(db_conn));
}

/** return values:
 * 0 - all ok
 * 1 - error occured
 */
int
db_file_add(const char *path, uuid_t *new_uuid)
{
  sqlite3_stmt *stmt = NULL;
  int ret = 0;

  if (sqlite3_prepare_v2(db_conn, SQL_F_ADD, strlen(SQL_F_ADD), &stmt, NULL) != SQLITE_OK)
    {
      msg(msg_warn, COMMON_ERR_FMTN, MSG_D_FAILPREPARE, sqlite3_errmsg(db_conn));
      return 1;
    }

  uuid_generate(new_uuid, path);

  /* this is safe, because sqlite interprets unbound values as NULL */
  if (new_uuid->id != 0)
    sqlite3_bind_int64(stmt, 1, new_uuid->id);
  sqlite3_bind_int(stmt, 2, new_uuid->dirname_hash);
  sqlite3_bind_text(stmt, 3, path, -1, SQLITE_STATIC);

  switch (sqlite3_step(stmt))
    {
      case SQLITE_DONE :
        new_uuid->id = (uint64_t) sqlite3_last_insert_rowid(db_conn);
        ASSERT(new_uuid->id != 0, MSG_D_ZEROUUID);
        break;
      default :
        msg(msg_warn, COMMON_ERR_FMTN, MSG_D_FAILEXEC, sqlite3_errmsg(db_conn));
        ret = 1;
        break;
    }

  sqlite3_finalize(stmt);

  return ret;
}

int
db_file_update(const char *path, uuid_t *uuid)
{
  sqlite3_stmt *stmt = NULL;
  size_t len = 0;

  len = strlen(SQL_F_UPDATE);
  if (sqlite3_prepare_v2(db_conn, SQL_F_UPDATE, len, &stmt, NULL) != SQLITE_OK)
    {
      msg(msg_warn, COMMON_ERR_FMTN, MSG_D_FAILPREPARE, sqlite3_errmsg(db_conn));
      return 1;
    }

  get_path_checksums(uuid, path);

  sqlite3_bind_int(stmt, 1, uuid->dirname_hash);
  sqlite3_bind_text(stmt, 2, path, -1, SQLITE_STATIC);
  sqlite3_bind_int64(stmt, 3, (sqlite3_int64) uuid->id);

  if (sqlite3_step(stmt) != SQLITE_DONE)
    {
      msg(msg_warn, COMMON_ERR_FMTN, MSG_D_FAILEXEC, sqlite3_errmsg(db_conn));
      sqlite3_finalize(stmt);
      return 1;
    }

  sqlite3_finalize(stmt);

  return 0;
}

int
db_file_del(const uuid_t *uuid)
{
  sqlite3_stmt *stmt = NULL;
  size_t len = 0;
  int ret = 0;

  len = strlen(SQL_F_DEL);
  if (sqlite3_prepare_v2(db_conn, SQL_F_DEL, len, &stmt, NULL) != SQLITE_OK)
    {
      msg(msg_warn, COMMON_ERR_FMTN, MSG_D_FAILPREPARE, sqlite3_errmsg(db_conn));
      return 1;
    }

  sqlite3_bind_int64(stmt, 1, (sqlite3_int64) uuid->id);

  if ((ret = sqlite3_step(stmt)) != SQLITE_DONE)
    msg(msg_warn, COMMON_ERR_FMTN, MSG_D_FAILEXEC, sqlite3_errmsg(db_conn));

  sqlite3_finalize(stmt);

  return (ret == SQLITE_DONE) ? 0 : 1;
}

/** return values:
 * 0 - all ok
 * 1 - error
 * note: 'path' must be PATH_MAX bytes in size
 */
int
db_file_get(const uuid_t *uuid, char *path)
{
  sqlite3_stmt *stmt = NULL;
  size_t len = 0;
  int ret = 0;

  len = strlen(SQL_F_GET);
  if (sqlite3_prepare_v2(db_conn, SQL_F_GET, len, &stmt, NULL) != SQLITE_OK)
    {
      msg(msg_warn, COMMON_ERR_FMTN, MSG_D_FAILPREPARE, sqlite3_errmsg(db_conn));
      return 1;
    }

  sqlite3_bind_int64(stmt, 1, (sqlite3_int64) uuid->id);

  while ((ret = sqlite3_step(stmt)) == SQLITE_ROW)
    snprintf(path, PATH_MAX, "%s", sqlite3_column_text(stmt, 0));

  if (ret != SQLITE_DONE)
    msg(msg_warn, COMMON_ERR_FMTN, MSG_D_FAILEXEC, sqlite3_errmsg(db_conn));

  sqlite3_finalize(stmt);

  return (ret == SQLITE_DONE) ? 0 : 1;
}

/** return values:
 * 0 - all ok
 * 1 - more data expected
 * 2 - error
 */
/**
 * NOTE: 'str' may contain '*' chars, if you want fuzzy search
 */
int
db_file_search_path(const char *str, query_limits_t *lim, list_t *results,
                    int (*cb)(const char *, const uuid_t *))
{
  sqlite3_stmt *stmt = NULL;
  uuid_t uuid = { 0, 0 };
  size_t len = 0;
  int ret = 0;
  int rows = 0;
  char buf[PATH_MAX + UUID_CHAR_LEN + 3] = { 0 };

  ASSERT(str != NULL && lim != NULL && (results != NULL || cb != NULL), MSG_M_NULLPTR);

  len = strlen(SQL_F_SEARCH);
  if (sqlite3_prepare_v2(db_conn, SQL_F_SEARCH, len, &stmt, NULL) != SQLITE_OK)
    {
      msg(msg_warn, COMMON_ERR_FMTN, MSG_D_FAILPREPARE, sqlite3_errmsg(db_conn));
      return 2;
    }

  if (results != NULL)
    list_clear(results);

  strncpy(buf, str, PATH_MAX + 1);
  len = strlen(buf);
  while (len --> 0)
    if (buf[len] == '*')
      buf[len] = '%';

  sqlite3_bind_text(stmt, 1, buf, -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 2, lim->limit);
  sqlite3_bind_int(stmt, 3, lim->offset);

  while ((ret = sqlite3_step(stmt)) == SQLITE_ROW)
    {
      rows++;
      uuid.id    = (uint64_t) sqlite3_column_int64(stmt, 0);
      uuid.dirname_hash = (uint16_t) sqlite3_column_int(stmt, 1);

      if (results != NULL)
        {
          len = snprintf_m_uuid_file(buf, PATH_MAX + UUID_CHAR_LEN + 3,
                                     &uuid, (char *) sqlite3_column_text(stmt, 2));
          list_item_add(results, buf, len);
        }

      if (cb != NULL)
        cb((const char *) sqlite3_column_text(stmt, 3), &uuid);
    }
  lim->offset += rows;

  if (ret != SQLITE_DONE)
    {
      msg(msg_warn, COMMON_ERR_FMTN, MSG_D_FAILEXEC, sqlite3_errmsg(db_conn));
      if (results != NULL)
        list_clear(results);
      sqlite3_finalize(stmt);

      return 2;
    }

  if (results != NULL)
    {
      if (results->items > 1)
        results->flags |= LIST_MULTI;
      results->type = LIST_M_UUID_FILE;
    }

  sqlite3_finalize(stmt);

  return (rows == lim->limit) ? 1 : 0;
}

/** return values:
 * 0 - all ok
 * 1 - more data expected
 * 2 - error
 */
int
db_file_search_tag(const search_t *search, query_limits_t *lim, list_t *results,
                   int (*cb)(const char *, const char *))
{
  sqlite3_stmt *stmt;
  size_t len = 0;
  int ret = 0;
  int rows = 0;
  int matches = 0;
  char buf[PATH_MAX] = { 0 };
  char *fts_query = NULL;

  ASSERT(search != NULL && lim != NULL && (results != NULL || cb != NULL), MSG_M_NULLPTR);

  len = strlen(SQL_T_SEARCH);
  if (sqlite3_prepare_v2(db_conn, SQL_T_SEARCH, len, &stmt, NULL) != SQLITE_OK)
    {
      msg(msg_warn, COMMON_ERR_FMTN, MSG_D_FAILPREPARE, sqlite3_errmsg(db_conn));
      return 2;
    }

  if (results != NULL)
    list_clear(results);

  fts_query = _db_get_fts_query(search);

  do /* loop, until we get 'limit' items or lesser than 'limit' rows from query */
    {
      sqlite3_bind_text(stmt, 1, fts_query, -1, SQLITE_TRANSIENT);
      sqlite3_bind_int(stmt,  2, lim->limit);
      sqlite3_bind_int(stmt,  3, lim->offset);

      for (rows = 0, matches = 0; (ret = sqlite3_step(stmt)) == SQLITE_ROW;)
        {
          rows++;
          /* NOTE: we already filter this by FTS query
          if (search_match_substr(search, (char *) sqlite3_column_text(stmt, 1)) < 1)
            continue;
          */
          if (search_match_exact(search, (char *) sqlite3_column_text(stmt, 1)) < 1)
            continue;
#ifdef REGEX_SEARCH
          if (search_match_regex(search, (char *) sqlite3_column_text(stmt, 1)) < 1)
            continue;
#endif
          matches++;

          if (results != NULL)
            {
              len = snprintf(buf, PATH_MAX, "%s", (char *) sqlite3_column_text(stmt, 0));
              list_item_add(results, buf, len);
            }

          if (cb != NULL)
            cb((char *) sqlite3_column_text(stmt, 0),
               (char *) sqlite3_column_text(stmt, 1));

            /* NOTE: if we simple wait until 'for' loop ends,     *
             * number of items in result will float around limit. */
          if (matches == lim->limit)
            break;
        }
      lim->offset += rows;
      sqlite3_reset(stmt);
      sqlite3_clear_bindings(stmt);
    }
  while (matches < lim->limit && rows == lim->limit);
  FREE(fts_query);

  if (ret != SQLITE_DONE && ret != SQLITE_ROW)
    {
      msg(msg_warn, COMMON_ERR_FMTN, MSG_D_FAILEXEC, sqlite3_errmsg(db_conn));
      if (results != NULL)
        list_clear(results);
      sqlite3_finalize(stmt);
      return 2;
    }

  if (results != NULL)
    {
      if (results->items > 1)
        results->flags |= LIST_MULTI;
      results->type = LIST_L_FILES;
    }

  sqlite3_finalize(stmt);

  return (results->items == lim->limit) ? 1 : 0;
}

/** return values:
 * 0 - all ok
 * 1 - general error
 * 2 - no such_uuid
 */
int
db_tags_get(uuid_t *uuid, list_t *tags)
{
  sqlite3_stmt *stmt = NULL;
  int ret = 0;

  ASSERT(uuid != NULL && tags != NULL, MSG_M_NULLPTR);

  if (tags->items > 0)
    list_clear(tags);

  if (sqlite3_prepare_v2(db_conn, SQL_T_GET, strlen(SQL_T_GET), &stmt, NULL) != SQLITE_OK)
    {
      msg(msg_warn, COMMON_ERR_FMTN, MSG_D_FAILPREPARE, sqlite3_errmsg(db_conn));
      return 1;
    }

  sqlite3_bind_int64(stmt, 1, (sqlite3_int64) uuid->id);

  switch (sqlite3_step(stmt))
    {
      case SQLITE_ROW :
        list_parse_tags(tags, (char *) sqlite3_column_text(stmt, 1));
        ret = 0;
        break;
      case SQLITE_DONE :
        ret = 2;
        break;
      default :
        msg(msg_warn, COMMON_ERR_FMTN, MSG_D_FAILEXEC, sqlite3_errmsg(db_conn));
        ret = 1;
        break;
    }

  sqlite3_finalize(stmt);

  return ret;
}

int
db_tags_set(const uuid_t *uuid, list_t *tags)
{
  sqlite3_stmt *stmt = NULL;
  size_t len = 0;
  int ret = 0;
  char *p = NULL;

  ASSERT(uuid != NULL && tags != NULL, MSG_M_NULLPTR);

  len = strlen(SQL_T_SET);
  if (sqlite3_prepare_v2(db_conn, SQL_T_SET, len, &stmt, NULL) != SQLITE_OK)
    {
      msg(msg_warn, COMMON_ERR_FMTN, MSG_D_FAILPREPARE, sqlite3_errmsg(db_conn));
      return 1;
    }

#ifdef UNIQ_TAGS_LIST
  db_tag_add_uniq(tags);
#endif

  CALLOC(p, tags->len + 2, sizeof(char));
  list_items_merge(tags, ' ');
  memcpy(&p[1], tags->buf, tags->len);
  p[0] = ' ';
  p[tags->len] = ' ';

  sqlite3_bind_int64(stmt, 1, (sqlite3_int64) uuid->id);
  sqlite3_bind_text(stmt,  2, p, tags->len + 2, SQLITE_STATIC);

  if ((ret = sqlite3_step(stmt)) != SQLITE_DONE)
    msg(msg_warn, COMMON_ERR_FMTN, MSG_D_FAILEXEC, sqlite3_errmsg(db_conn));

  FREE(p);
  sqlite3_finalize(stmt);

  return (ret == SQLITE_DONE) ? 0 : 1;
}

int
db_tags_clr(const uuid_t *uuid)
{
  sqlite3_stmt *stmt = NULL;
  size_t len = 0;
  int ret = 0;

  ASSERT(uuid != NULL, MSG_M_NULLPTR);

  len = strlen(SQL_T_CLR);
  if (sqlite3_prepare_v2(db_conn, SQL_T_CLR, len, &stmt, NULL) != SQLITE_OK)
    {
      msg(msg_warn, COMMON_ERR_FMTN, MSG_D_FAILPREPARE, sqlite3_errmsg(db_conn));
      return 1;
    }

  sqlite3_bind_int64(stmt, 1, (sqlite3_int64) uuid->id);

  if ((ret = sqlite3_step(stmt)) != SQLITE_DONE)
    msg(msg_warn, COMMON_ERR_FMTN, MSG_D_FAILEXEC, sqlite3_errmsg(db_conn));

  sqlite3_finalize(stmt);

  return (ret == SQLITE_DONE) ? 0 : 1;
}

int
db_tag_add_uniq(list_t *tags)
{
  sqlite3_stmt *stmt = NULL;
  uint32_t hash = 0;
  size_t len = 0;
  int ret = 0;
  char *p = NULL;

  ASSERT(tags != NULL, MSG_M_NULLPTR);

  len = strlen(SQL_T_UNIQ_ADD);
  if (sqlite3_prepare_v2(db_conn, SQL_T_UNIQ_ADD, len, &stmt, NULL) != SQLITE_OK)
    {
      msg(msg_warn, COMMON_ERR_FMTN, MSG_D_FAILPREPARE, sqlite3_errmsg(db_conn));
      return 1;
    }

  while (list_items_walk(tags, &p) > 0)
    {
      len = strlen(p);
      hash = jhash(p, len);
      sqlite3_bind_int(stmt, 1, hash);
      sqlite3_bind_text(stmt, 2, p, len, SQLITE_STATIC);

      if ((ret = sqlite3_step(stmt)) != SQLITE_DONE)
        {
          msg(msg_warn, COMMON_ERR_FMTN, MSG_D_FAILEXEC, sqlite3_errmsg(db_conn));
          break;
        }

      sqlite3_clear_bindings(stmt);
      sqlite3_reset(stmt);
    }

  sqlite3_finalize(stmt);

  return (ret == SQLITE_DONE) ? 0 : 1;
}

/** return values:
 * 0 - all ok
 * 1 - more data expected
 * 2 - error
 */
int
db_tags_find(const char *str, query_limits_t *lim, list_t *results)
{
  sqlite3_stmt *stmt = NULL;
  char buf[PATH_MAX]; /* not exactly 'path', but size should be reasonable */
  char *p = NULL;
  size_t len = 0;
  int ret = 0;

  ASSERT(str != NULL && lim != NULL && results != NULL, MSG_M_NULLPTR);

  len = strlen(SQL_T_FIND);
  if (sqlite3_prepare_v2(db_conn, SQL_T_FIND, len, &stmt, NULL) != SQLITE_OK)
    {
      msg(msg_warn, COMMON_ERR_FMTN, MSG_D_FAILPREPARE, sqlite3_errmsg(db_conn));
      return 2;
    }

  if (results != NULL)
    list_clear(results);

  strncpy(buf, str, PATH_MAX);
  for (p = buf; *p != '\0'; p++)
    if (*p == '*') *p = '%';

  sqlite3_bind_text(stmt, 1, buf, len, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 2, lim->limit);
  sqlite3_bind_int(stmt, 3, lim->offset);

  while ((ret = sqlite3_step(stmt)) == SQLITE_ROW)
    {
      len = snprintf(buf, PATH_MAX, "%s", (char *) sqlite3_column_text(stmt, 0));
      list_item_add(results, buf, len);
      lim->offset++;
    }

  if (ret != SQLITE_DONE)
    {
      msg(msg_warn, COMMON_ERR_FMTN, MSG_D_FAILEXEC, sqlite3_errmsg(db_conn));
      sqlite3_finalize(stmt);
      list_clear(results);
      return 2;
    }

  sqlite3_finalize(stmt);

  return (results->items == MAX_QUERY_LIMIT) ? 1 : 0;
}

void
db_commit(void)
{
#ifdef ASYNC_DB_WRITE
  sqlite3_exec(db_conn, "COMMIT TRANSACTION;", NULL, NULL, NULL);
  sqlite3_exec(db_conn, "BEGIN  TRANSACTION;", NULL, NULL, NULL);
#endif
}
