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
  size_t len;
  char *buf;
} data_t;
