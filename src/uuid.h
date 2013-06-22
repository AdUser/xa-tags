#define UUID_CHAR_LEN 16 /* sizeof(uint64_t) * 2 */
#define UUID_FORMAT "%08X%08X" /* base, counter */

typedef uint64_t uuid_t;

/** API functions */
/** common functions */
int uuid_generate(uuid_t *uuid, const char *path);

/** specific parts of uuid */
/* id */
int uuid_id_validate(char *uuid);
int uuid_id_parse(uuid_t *uuid, char *str);
char *uuid_id_printf(const uuid_t *uuid);
