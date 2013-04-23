-- db creation statements (v2)

-- service table
DROP TABLE IF EXISTS s_info;
CREATE TABLE s_info (
  version   INTEGER NOT NULL DEFAULT 2,
  uuid_min  INTEGER NOT NULL DEFAULT 0  -- start of uuid range
);

-- files location
DROP TABLE IF EXISTS d_files;
CREATE TABLE d_files (
  file_id      INTEGER PRIMARY KEY AUTOINCREMENT, -- sqlite rowid and uuid
  dirname_hash INTEGER NOT NULL DEFAULT 0,
  filename     TEXT    NOT NULL DEFAULT ''
);
CREATE INDEX IF NOT EXISTS i_dirname_hash ON d_files (dirname_hash);

-- tags for files
CREATE VIRTUAL TABLE d_tags USING fts4 (tags, tokenize=simple);
CREATE VIRTUAL TABLE d_tags_idx USING fts4aux (d_tags);

-- aux table, used for autocompletion
DROP TABLE IF EXISTS d_tags_uniq;
CREATE TABLE d_tags_uniq (
  tag_id    INTEGER NOT NULL DEFAULT 0,
  tag       TEXT    NOT NULL DEFAULT '',
  PRIMARY KEY (tag_id)
);

-- init database
INSERT INTO s_info (version, uuid_min) VALUES(1, abs(random() / 2));
INSERT INTO d_files (file_id) SELECT uuid_min FROM s_info;
