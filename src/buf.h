typedef struct buf_t
{
  size_t len;
  char *buf;
} buf_t;

void buf_extend(buf_t *buf, char *str, size_t str_len);
void buf_reduce(buf_t *buf, size_t len);
