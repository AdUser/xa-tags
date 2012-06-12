typedef struct
  {
    char key[16];
    char *value;
  }
data_item_t;

typedef struct
  {
    char blk_name[16];
    unsigned int items_cnt;
    data_item_t **items;
  }
data_blk_t;

typedef struct
  {
    enum { OBJ_FILE, OBJ_TAG, OBJ_DB } object;
    char *operation;
    int data_blks_cnt;
    data_blk_t **data;
  }
rpc_request_t;

typedef struct
  {
    enum { ST_OK, ST_ERR } status;
    char *msg;
    int data_blks_cnt;
    data_blk_t **data;
  }
rpc_responce_t;

