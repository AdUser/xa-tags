/** typedefs */

/** request */
typedef enum req_type_t
  {
    REQ_NONE, /* invalid or missing request */
    REQ_FILE,
    REQ_TAG,
    REQ_DB,
    REQ_HELLO,
    REQ_BYE,
    REQ_HELP
  } req_type_t;

typedef enum req_op_t
  {
    OP_NONE = 0,

    /* FILE operations */
    OP_F_ADD  = 1,
    OP_F_DEL,
    OP_F_SEARCH,

    /* TAG operations */
    OP_T_ADD = 11,
    OP_T_DEL,
    OP_T_GET,
    OP_T_SET,
    OP_T_CLR,

    /* DB operations */
    OP_D_STAT = 21,
    OP_D_CHECK
  } req_op_t;

typedef struct ipc_req_t
  {
    req_type_t type;
    req_op_t op;
    data_t data;
  } ipc_req_t;

/** response */
typedef enum resp_status_t
  {
    STATUS_OK = 0,
    STATUS_ERR
  } resp_status_t;

typedef struct ipc_resp_t
  {
    resp_status_t status;
    data_t data;
  } ipc_resp_t;

/** API functions */
int parse_buf(conn_t *conn, ipc_req_t *req, char *buf, size_t buf_len);
int validate_data(data_t *data);
