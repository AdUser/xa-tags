int file_tags_get(const char *path, data_t *tags);
int file_tags_set(const char *path, data_t *tags);
int file_tags_clr(const char *path);

int file_uuid_get(const char *path, uuid_t *uuid);
int file_uuid_set(const char *path, const uuid_t *uuid);
int file_uuid_clr(const char *path);

int file_xattr_supported(const char *path);
