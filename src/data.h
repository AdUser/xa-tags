/** data */
#define DATA_MAX_ITEMS 255

/* data flags */
#define DATA_MULTI          1 <<  0

typedef struct data_t
{
  uint8_t flags;
  uint8_t items;
  enum
    {
      DATA_EMPTY = 0,
      DATA_T_MSG,
      DATA_L_FILES,
      DATA_L_UUIDS,
      DATA_M_UUID_TAGS,
      DATA_M_UUID_FILE
    } type;
  size_t len; /* len assumes '(item + \0) * items, but without extra \0' */
  char *buf; /* [item 1\0item2\0\0] */
} data_t;

/** API functions */
int data_validate(data_t *data, data_t *errors, int strict);
int data_item_add(data_t *data, char *item, size_t item_len);
void data_clear(data_t *data);
