/** list */
#define LIST_MAX_ITEMS 255

/* list flags */
#define LIST_MULTI          1 <<  0

typedef struct list_t
{
  uint8_t flags;
  uint8_t items;
  enum
    {
      LIST_EMPTY = 0,
      LIST_T_MSG,
      LIST_L_FILES,
      LIST_L_UUIDS,
      LIST_L_TAGS,
      LIST_M_UUID_TAGS,
      LIST_M_UUID_FILE
    } type;
  size_t len;  /* assumes '(item + \0) * items */
  size_t size; /* available size in buffer (bytes) */
  char *buf; /* [item 1\0item2\0] or NULL */
} list_t;

/** API functions */
int list_validate(list_t *list, list_t *errors, int strict);
int list_item_add(list_t *list, char *item, size_t item_len);
int list_item_del(list_t *list, char *item);
int list_items_walk(const list_t *list, char **item);
char *list_item_search(list_t *list, const char *item);
void list_items_split(list_t *list, char delim);
void list_items_merge(list_t *list, char glue);
void list_clear(list_t *list);
void list_copy(list_t *to, const list_t *from);
int list_parse_tags(list_t *list, const char *str);
void list_merge(list_t *to, list_t *from);
