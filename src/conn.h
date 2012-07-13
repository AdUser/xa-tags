typedef struct conn_t
  {
    int fd;
    int timeout;
    char        *rd_buf;
    char        *wr_buf;
    unsigned int rd_buf_len;
    unsigned int wr_buf_len;
  } conn_t;
