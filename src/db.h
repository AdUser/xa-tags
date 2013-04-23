#define DB_USER_PATH ".local/cache/" PROGNAME "/"
#define DB_SYSTEM_PATH "/var/lib/" PROGNAME "/"
#define DB_FILENAME "xa-tags.db"
#define DB_VERSION "1"
#define INFO_TABLE "s_info"
#define FILE_TABLE "d_files"
#define TAGS_TABLE "d_uniq_tags"

#define UUID_COL "file_id, crc_dname, crc_fname"

/* FILE request type */
#define SQL_F_ADD \
  "INSERT INTO " FILE_TABLE " (file_id, crc_dname, crc_fname, filename) VALUES (?, ?, ?, ?)"
#define SQL_F_DEL \
  "DELETE FROM " FILE_TABLE " WHERE file_id = ?;"
#define SQL_F_GET \
  "SELECT filename FROM " FILE_TABLE " WHERE file_id = ?"
#define SQL_F_SEARCH \
  "SELECT " UUID_COL ", filename FROM " FILE_TABLE " WHERE filename LIKE ? LIMIT ? OFFSET ?"
#define SQL_F_UPDATE \
  "UPDATE " FILE_TABLE " SET crc_dname = ?, crc_fname = ?, filename = ? WHERE file_id = ?"

/* TAG request type */
#define SQL_T_GET \
  "SELECT " UUID_COL ", tags FROM " FILE_TABLE " WHERE file_id = ?"
#define SQL_T_SET \
  "UPDATE " FILE_TABLE " SET tags = ? WHERE file_id = ?"
#define SQL_T_CLR \
  "DELETE FROM " TAGS_TABLE " WHERE rowid = ?"
#define SQL_T_SEARCH \
  "SELECT filename, tags FROM " FILE_TABLE " WHERE tags LIKE ? LIMIT ? OFFSET ?"
#define SQL_T_FIND \
  "SELECT tag FROM " TAGS_TABLE " WHERE tag LIKE ? LIMIT ? OFFSET ?"
/* service operations */
#define SQL_T_UNIQ_ADD \
  "INSERT OR IGNORE INTO " TAGS_TABLE " (tag_id, tag) VALUES (?, ?)"

/* DB request type */
#define SQL_D_STAT \
  "SELECT " \
    "(SELECT COUNT(*) FROM " TAGS_TABLE ") as 'uniq_tags', " \
    "(SELECT COUNT(*) FROM " FILE_TABLE ") as 'file_records'"

/* DB create statements */
#define SQL_DB_CREATE_COMMON \
"CREATE TABLE " INFO_TABLE \
"(" \
"  version  INTEGER NOT NULL DEFAULT 0," \
"  uuid_min INTEGER NOT NULL DEFAULT 0 " \
");" \
"CREATE TABLE " FILE_TABLE \
"(" \
"  file_id   INTEGER PRIMARY KEY AUTOINCREMENT," \
"  crc_dname INTEGER NOT NULL DEFAULT 0,"  \
"  crc_fname INTEGER NOT NULL DEFAULT 0,"  \
"  filename  TEXT    NOT NULL DEFAULT ''," \
"  tags      TEXT    NOT NULL DEFAULT '' " \
");" \
"CREATE INDEX IF NOT EXISTS i_crc_dname ON " FILE_TABLE " (crc_dname);" \
"CREATE INDEX IF NOT EXISTS i_crc_fname ON " FILE_TABLE " (crc_fname);"

#define SQL_DB_CREATE_UNIQ \
"CREATE TABLE " TAGS_TABLE \
"(" \
"  tag_id INTEGER NOT NULL DEFAULT 0,"  \
"  tag    TEXT    NOT NULL DEFAULT ''," \
"  PRIMARY KEY (tag_id)" \
");"

#define SQL_DB_INIT \
"INSERT INTO " INFO_TABLE " (version, uuid_min) VALUES (" DB_VERSION ", abs(random() / 2));" \
"INSERT INTO " FILE_TABLE " (file_id, filename) SELECT uuid_min, '__PLACEHOLDER__' FROM " INFO_TABLE ";" \

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
int db_file_search_path(const char *str, query_limits_t *lim, data_t *results,
                        int (*cb)(const char *, const uuid_t *));
int db_file_search_tag(const data_t *tags, query_limits_t *lim, data_t *results,
                       int (*cb)(const char *, const char *));

int db_tags_get(uuid_t *uuid, data_t *tags);
int db_tags_set(const uuid_t *uuid, data_t *tags);
int db_tags_clr(const uuid_t *uuid);
int db_tags_find(const char *str, query_limits_t *lim, data_t *results);

#ifdef UNIQ_TAGS_LIST
int db_tag_add_uniq(data_t *tags);
#endif

void db_commit(void);

void db_open(void);
void db_close(void);
