/** connection flags */
#define CONN_F_HUMAN     1 <<  0 /* enable b/j and hookers */

typedef struct conn_t
  {
    int fd;
    int timeout;
    char        *rd_buf;
    char        *wr_buf;
    unsigned int rd_buf_len;
    unsigned int wr_buf_len;
    uint16_t flags;
    data_t errors;
  } conn_t;
