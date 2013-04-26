/** connection flags */
#define CONN_F_HUMAN     1 <<  0 /* enable b/j and hookers */
#define CONN_F_NOBATCH   1 <<  1 /* disable batch requests */
#define CONN_F_STRICT    1 <<  2 /* don't process requests with any errors */
#define CONN_F_CLOSE     1 <<  3 /* gracefully close the connection */

typedef struct conn_t
  {
    int fd;
    int timeout;

    buf_t rd;
    buf_t wr;

    /* used only by server */
    uint16_t flags;
    list_t errors;
  } conn_t;

/** API functions */

void conn_on_errors(conn_t *conn);
void conn_on_read(conn_t *conn);
void conn_on_write(conn_t *conn);
