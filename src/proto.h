#define MAX_ITEM_KEY_SIZE   16
#define MAX_ITEM_VAL_SIZE 4096

#define MAX_BLK_NAME_SIZE   16

typedef struct
  {
    char key[MAX_ITEM_KEY_SIZE];
    char *value;
  }
data_item_t;

typedef struct
  {
    char blk_name[MAX_BLK_NAME_SIZE];
    unsigned int items_used;
    unsigned int items_total;
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
ipc_request_t;

typedef struct
  {
    enum { ST_OK, ST_ERR } status;
    char *msg;
    int data_blks_cnt;
    data_blk_t **data;
  }
ipc_responce_t;

data_item_t *data_item_create(char *key, char *value);

data_blk_t  *data_blk_create(char *name, unsigned int items);
data_blk_t  *data_blk_item_add(data_blk_t *blk, data_item_t *item);
data_blk_t  *data_blk_item_del(data_blk_t *blk, char *key, bool all);
data_blk_t  *data_blk_gc(data_blk_t *blk);

int ipc_data_blk_add(unsigned int *cnt, data_blk_t ***data);
int ipc_data_blk_del(unsigned int *cnt, char *blk_name);
