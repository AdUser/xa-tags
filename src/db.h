#define DB_USER_PATH ".local/cache/" PROGNAME "/"
#define DB_SYSTEM_PATH "/var/lib/" PROGNAME "/"
#define DB_FILENAME "xa-tags.db"
#define DB_VERSION "2"
#define INFO_TABLE "s_info"
#define FILE_TABLE "d_files"
#define TAGS_TABLE "d_tags"
#define IDX_TAGS_TABLE "d_tags_idx"
#define UNIQ_TAGS_TABLE "d_tags_uniq"

#define UUID_COL "file_id, dirname_hash"

/* FILE request type */
#define SQL_F_ADD \
  "INSERT INTO " FILE_TABLE " (file_id, dirname_hash, filename) VALUES (?, ?, ?)"
#define SQL_F_DEL \
  "DELETE FROM " FILE_TABLE " WHERE file_id = ?;"
#define SQL_F_GET \
  "SELECT filename FROM " FILE_TABLE " WHERE file_id = ?"
#define SQL_F_SEARCH \
  "SELECT " UUID_COL ", filename FROM " FILE_TABLE " WHERE filename LIKE ? LIMIT ? OFFSET ?"
#define SQL_F_UPDATE \
  "UPDATE " FILE_TABLE " SET dirname_hash = ?, filename = ? WHERE file_id = ?"

/* TAG request type */
#define SQL_T_GET \
  "SELECT rowid, tags FROM " TAGS_TABLE " WHERE rowid = ?"
#define SQL_T_SET \
  "INSERT OR REPLACE INTO " TAGS_TABLE " (rowid, tags) VALUES (?, ?)"
#define SQL_T_CLR \
  "DELETE FROM " TAGS_TABLE " WHERE rowid = ?"
#define SQL_T_SEARCH \
  "SELECT filename, tags FROM " FILE_TABLE " JOIN " TAGS_TABLE \
  " ON " FILE_TABLE ".file_id = " TAGS_TABLE ".rowid" \
  " WHERE tags MATCH ? LIMIT ? OFFSET ?"
#ifdef UNIQ_TAGS_LIST
#define SQL_T_FIND \
  "SELECT tag FROM " UNIQ_TAGS_TABLE " WHERE tag LIKE ? LIMIT ? OFFSET ?"
#endif
/* service operations */
#define SQL_T_UNIQ_ADD \
  "INSERT OR IGNORE INTO " UNIQ_TAGS_TABLE " (tag_id, tag) VALUES (?, ?)"

/* DB request type */
#define SQL_D_STAT \
  "SELECT " \
    "(SELECT COUNT(*) FROM " UNIQ_TAGS_TABLE ") as 'uniq_tags', " \
    "(SELECT COUNT(*) FROM " FILE_TABLE ") as 'file_records'"

/* DB create statements */
#define SQL_DB_CREATE_1 \
  "DROP TABLE IF EXISTS " INFO_TABLE ";" \
  "CREATE TABLE " INFO_TABLE " (" \
  "  version   INTEGER NOT NULL DEFAULT 1," \
  "  uuid_min  INTEGER NOT NULL DEFAULT 0"  \
  ");"

#define SQL_DB_CREATE_2 \
  "DROP TABLE IF EXISTS " FILE_TABLE ";" \
  "CREATE TABLE " FILE_TABLE " (" \
  "  file_id      INTEGER PRIMARY KEY AUTOINCREMENT," \
  "  dirname_hash INTEGER NOT NULL DEFAULT 0," \
  "  filename     TEXT    NOT NULL DEFAULT ''" \
  ");" \
  "CREATE INDEX IF NOT EXISTS i_dirname_hash ON " FILE_TABLE " (dirname_hash);"

#define SQL_DB_CREATE_3 \
  "DROP TABLE IF EXISTS " TAGS_TABLE ";" \
  "CREATE VIRTUAL TABLE " TAGS_TABLE " USING fts4 (tags, tokenize=simple);" \
  "CREATE VIRTUAL TABLE " IDX_TAGS_TABLE " USING fts4aux (d_tags);"

#define SQL_DB_CREATE_UNIQ \
  "DROP TABLE IF EXISTS " UNIQ_TAGS_TABLE ";" \
  "CREATE TABLE " UNIQ_TAGS_TABLE \
  "(" \
  "  tag_id INTEGER NOT NULL DEFAULT 0,"  \
  "  tag    TEXT    NOT NULL DEFAULT ''," \
  "  PRIMARY KEY (tag_id)" \
  ");"

#define SQL_DB_INIT \
  "INSERT INTO " INFO_TABLE " (version, uuid_min) VALUES (" DB_VERSION ", abs(random() / 2));" \
  "INSERT INTO " FILE_TABLE " (file_id, filename) SELECT uuid_min, '' FROM " INFO_TABLE ";" \

#define SQL_DB_CHECKVERSION \
  "SELECT version == " DB_VERSION " FROM " INFO_TABLE ";"

#define MAX_QUERY_LIMIT 250

typedef struct query_limits_t
{
  uint64_t offset;
  uint16_t limit;
} query_limits_t;

/** API functions */

char *db_find_path_user(void);
char *db_find_path_system(void);

int db_file_add(const char *path, uuid_t *new_uuid);
int db_file_update(const char *path, uuid_t *uuid);
int db_file_del(const uuid_t *uuid);
int db_file_get(const uuid_t *uuid, char *path);
int db_file_search_path(const char *str, query_limits_t *lim, list_t *results,
                        int (*cb)(const char *, const uuid_t *));
int db_file_search_tag(const search_t *search, query_limits_t *lim, list_t *results,
                       int (*cb)(const char *, const char *));

int db_tags_get(uuid_t *uuid, list_t *tags);
int db_tags_set(const uuid_t *uuid, list_t *tags);
int db_tags_clr(const uuid_t *uuid);

#ifdef UNIQ_TAGS_LIST
int db_tags_find(const char *str, query_limits_t *lim, list_t *results);
int db_tag_add_uniq(list_t *tags);
#endif

void db_commit(void);

void db_open(void);
void db_close(void);
