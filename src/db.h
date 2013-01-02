#define DB_USER_PATH ".local/cache/" PROGNAME "/"
#define DB_SYSTEM_PATH "/var/lib/" PROGNAME "/"
#define DB_FILENAME "xa-tags.db"
#define DB_VERSION "2"
#define INFO_TABLE "s_info"
#define MAIN_TABLE "d_files"
#define TAGS_TABLE "d_uniq_tags"

#define UUID_COL "file_id, crc_dname, crc_fname"

/* FILE request type */
#define SQL_F_ADD \
  "INSERT INTO " MAIN_TABLE " (crc_dname, crc_fname, filename) VALUES (?, ?, ?)"
#define SQL_F_DEL \
  "DELETE FROM " MAIN_TABLE " WHERE file_id = ?;"
#define SQL_F_GET \
  "SELECT filename FROM " MAIN_TABLE " WHERE file_id = ?"
#define SQL_F_SEARCH \
  "SELECT " UUID_COL ", filename FROM " MAIN_TABLE " WHERE filename LIKE ?"
#define SQL_F_UPDATE \
  "UPDATE " MAIN_TABLE " SET crc_dname = ?, crc_fname = ?, filename = ? WHERE file_id = ?"

/* TAG request type */
#define SQL_T_GET \
  "SELECT " UUID_COL ", tags FROM " MAIN_TABLE " WHERE file_id = ?"
#define SQL_T_SET \
  "UPDATE " MAIN_TABLE " SET tags = ? WHERE file_id = ?"
#define SQL_T_SEARCH \
  "SELECT filename, tags FROM " MAIN_TABLE " WHERE tags LIKE ?"
#define SQL_T_FIND \
  "SELECT tag FROM " TAGS_TABLE " WHERE tag LIKE ?"
/* service operations */
#define SQL_T_UNIQ_ADD \
  "INSERT OR IGNORE INTO " TAGS_TABLE " (tag_id, tag) VALUES (?, ?)"

/* DB request type */
#define SQL_D_STAT \
  "SELECT " \
    "(SELECT COUNT(*) FROM " TAGS_TABLE ") as 'uniq_tags', " \
    "(SELECT COUNT(*) FROM " MAIN_TABLE ")     as 'file_records'"

/* DB create statements */
#define SQL_DB_CREATE_COMMON \
"CREATE TABLE " INFO_TABLE \
"(" \
"  version  INTEGER NOT NULL DEFAULT 0," \
"  uuid_min INTEGER NOT NULL DEFAULT 0 " \
");" \
"CREATE TABLE " MAIN_TABLE \
"(" \
"  file_id   INTEGER PRIMARY KEY," \
"  crc_dname INTEGER NOT NULL DEFAULT 0,"  \
"  crc_fname INTEGER NOT NULL DEFAULT 0,"  \
"  filename  TEXT    NOT NULL DEFAULT ''," \
"  tags      TEXT    NOT NULL DEFAULT '' " \
");" \
"CREATE INDEX IF NOT EXISTS i_crc_dname ON " MAIN_TABLE " (crc_dname);" \
"CREATE INDEX IF NOT EXISTS i_crc_fname ON " MAIN_TABLE " (crc_fname);"

#define SQL_DB_CREATE_UNIQ \
"CREATE TABLE " TAGS_TABLE \
"(" \
"  tag_id INTEGER NOT NULL DEFAULT 0,"  \
"  tag    TEXT    NOT NULL DEFAULT ''," \
"  PRIMARY KEY (tag_id)" \
");"

#define SQL_DB_INIT \
"INSERT INTO " INFO_TABLE " (version, uuid_min) VALUES (" DB_VERSION ", abs(random() / 2));" \
"INSERT INTO " MAIN_TABLE " (file_id, filename) SELECT uuid_min, '__PLACEHOLDER__' FROM " INFO_TABLE ";" \

/** API functions */

char *db_find_path_user(void);
char *db_find_path_system(void);

int db_file_add(const char *path, uuid_t *new_uuid);
int db_file_update(const char *path, uuid_t *uuid);
int db_file_del(const uuid_t *uuid);
int db_file_get(const uuid_t *uuid, data_t *results);
int db_file_search_path(const char *str, data_t *results);
int db_file_search_tag(char *str, data_t *results);

int db_tags_get(uuid_t *uuid, data_t *tags);
int db_tags_set(uuid_t *uuid, data_t *tags);
int db_tags_find(const char *str, data_t *results);

#ifdef UNIQ_TAGS_LIST
int db_tag_add_uniq(data_t *tags);
#endif

void db_commit(void);

void db_open(void);
void db_close(void);
