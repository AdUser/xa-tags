-- db creation statements (v1)

-- service table
DROP TABLE IF EXISTS s_info;
CREATE TABLE s_info (
  version   INTEGER NOT NULL DEFAULT 1,
  uuid_min  INTEGER NOT NULL DEFAULT 0  -- start of uuid range
);

-- main data table
DROP TABLE IF EXISTS d_files;
CREATE TABLE d_files (
  file_id   INTEGER PRIMARY KEY AUTOINCREMENT, -- sqlite rowid and part of uuid
  crc_dname INTEGER NOT NULL DEFAULT 0,        -- also part of uuid
  crc_fname INTEGER NOT NULL DEFAULT 0,
  filename  TEXT    NOT NULL DEFAULT '',
  tags      TEXT    NOT NULL DEFAULT ''
);
CREATE INDEX IF NOT EXISTS i_crc_dname ON d_files (crc_dname);
CREATE INDEX IF NOT EXISTS i_crc_fname ON d_files (crc_fname);

-- aux table, used for autocompletion
DROP TABLE IF EXISTS d_uniq_tags;
CREATE TABLE d_uniq_tags (
  tag_id    INTEGER NOT NULL DEFAULT 0,
  tag       TEXT    NOT NULL DEFAULT '',
  PRIMARY KEY (tag_id)
);

-- init database
INSERT INTO s_info (version, uuid_min) VALUES(1, abs(random() / 2));
INSERT INTO d_files (file_id) SELECT uuid_min FROM s_info;
DELETE FROM d_files;
