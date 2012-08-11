#define MAIN_TABLE "d_files"
#define TAGS_TABLE "d_uniq_tags"

#define UUID_COL "file_id, crc_dname, crc_fname"

/* FILE request type */
#define SQL_F_ADD \
  "INSERT INTO " MAIN_TABLE " (crc_dname, crc_fname, filename) VALUES (?, ?, ?)"
#define SQL_F_DEL \
  "DELETE FROM " MAIN_TABLE " WHERE file_id = ?;"
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
  "SELECT " UUID_COL ", filename FROM " MAIN_TABLE " WHERE tags LIKE ?"
#define SQL_T_FIND \
  "SELECT tag FROM " TAGS_TABLE " WHERE tag LIKE ?"
/* service operations */
#define SQL_T_UNIQ_ADD \
  "INSERT INTO " TAGS_TABLE " (tag_id, tag) VALUES (?, ?)"

/* DB request type */
#define SQL_D_STAT \
  "SELECT " \
    "(SELECT COUNT(*) FROM d_uniq_tags) as 'uniq_tags', " \
    "(SELECT COUNT(*) FROM d_files)     as 'file_records'"

/* DB create statements */
#define SQL_DB_CREATE \
"CREATE TABLE s_info ( version INT(2) NOT NULL DEFAULT 1);" \
"CREATE TABLE d_files ( file_id INTEGER PRIMARY KEY AUTOINCREMENT," \
" crc_dname INTEGER NOT NULL DEFAULT 0, crc_fname INTEGER NOT NULL DEFAULT 0,"\
" filename TEXT NOT NULL DEFAULT '', tags TEXT NOT NULL DEFAULT '');" \
"CREATE INDEX IF NOT EXISTS i_crc_dname ON d_files (crc_dname);" \
"CREATE INDEX IF NOT EXISTS i_crc_fname ON d_files (crc_fname);" \
"CREATE TABLE d_uniq_tags ( tag_id INTEGER NOT NULL DEFAULT 0, " \
"tag TEXT NOT NULL DEFAULT '', PRIMARY KEY (tag_id));"

/** API functions */

int db_file_add(char *path, uuid_t *new_uuid);
int db_file_update(char *path, uuid_t *new_uuid);
int db_file_del(uuid_t uuid);
int db_file_search_path(char *str, data_t *results);
int db_file_search_tag(char *str, data_t *results);

int db_tag_set(uuid_t uuid, char *tags);
int db_tag_get(uuid_t uuid, char **tags);
int db_tag_find(char *str, data_t *results);

void db_commit(void);

void db_open(void);
void db_close(void);
