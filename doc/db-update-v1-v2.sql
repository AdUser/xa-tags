-- update schema to v2

-- create table with fulltext search
CREATE VIRTUAL TABLE d_tags USING fts4(tags, tokenize=simple);

-- create table for fts index access
CREATE VIRTUAL TABLE d_tags_idx USING fts4aux (d_tags);

-- move tags to FTS table
INSERT INTO d_tags(docid, tags) SELECT rowid,tags from d_files;

-- sqlite don't support 'drop column' in 'alter table',
-- so this is workaround

DROP INDEX IF EXISTS i_crc_dname;
DROP INDEX IF EXISTS i_crc_fname;

-- create new table
CREATE TABLE d_files_new
(
  file_id      INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
  dirname_hash INTEGER NOT NULL DEFAULT 0,
  filename     TEXT    NOT NULL DEFAULT ''
);

-- move file records
INSERT INTO d_files_new SELECT file_id, 0, filename FROM d_files;

-- drop old table
DROP TABLE d_files;
ALTER TABLE d_files_new RENAME TO d_files;

-- rename uniq tags table
ALTER TABLE d_uniq_tags RENAME TO d_tags_uniq;

-- recreate indexes
CREATE INDEX i_dirname_hash ON d_files (dirname_hash);

-- update version
UPDATE s_info SET version = 2;

-- final cleanup
VACUUM;
