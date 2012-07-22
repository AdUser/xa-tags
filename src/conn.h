/** connection flags */
#define CONN_F_HUMAN     1 <<  0 /* enable b/j and hookers */
#define CONN_F_NOBATCH   1 <<  1 /* disable batch requests */
#define CONN_F_STRICT    1 <<  2 /* don't process requests with any errors */

typedef struct conn_t
  {
    int fd;
    int timeout;
    char        *rd_buf;
    char        *wr_buf;
    unsigned int rd_buf_len;
    unsigned int wr_buf_len;

    /* used only by server */
    uint16_t flags;
    data_t errors;
  } conn_t;

/** API functions */

void conn_buf_extend(conn_t *conn, char b, char *buf, size_t buf_len);
void conn_buf_reduce(conn_t *conn, char b, size_t len);
