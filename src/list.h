/** list */
#define LIST_MAX_ITEMS 255

/** list flags */
#define LIST_MULTI          1 <<  0  /**< @deprecated  Use list->items instead */

/**
 @struct list_t list.h
 @brief  General-purpose list implementation.

   Optimized for work with fixed-lenght strings and
 their fast split/join/item search.
   Memory are allocated by 512b blocks, that avoids realloc()
 on each add/del operation.
   List optionally may be speed up with index, which will
 be automatically dropped on first list change.
 */
typedef struct list_t
{
  uint8_t flags;  /**< list flags */
  uint8_t items;  /**< number of items in list  */
  enum
    {
      LIST_EMPTY = 0,    /**< list is empty or not properly set  */
      LIST_T_MSG,        /**< arbitrary string(s) */
      LIST_L_FILES,      /**< string(s), contains at least one symbol '/' */
      LIST_L_UUIDS,      /**< fixed-lenght string(s), hex-encoded */
      LIST_L_TAGS,       /**< string(s), with one or more token(s),
                              delimited by space */
      LIST_M_UUID_TAGS,  /**< string(s) with UUID and TAGS pair */
      LIST_M_UUID_FILE   /**< string(s) with UUID and FILE pair */
    } type;              /**< type of data in list  */
  char  **idx_items;     /**< index of items */
  size_t *idx_items_len; /**< array of lenght of items */
  size_t len;            /**< used size of buffer (assumes '(item + \0) * items) */
  size_t size;           /**< available size in buffer (bytes) */
  char *buf;             /**< contains string in "item 1\0item2\0" format or NULL */
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

/* items index manipulation */
void list_idx_drop(list_t *list);
int list_idx_create(list_t *list);
