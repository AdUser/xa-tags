#define UUID_CHAR_LEN 16 /* sizeof(uint64_t) * 2 */
#define UUID_FORMAT "%08X%08X" /* base, counter */
#define HASH_CHAR_LEN  8 /* sizeof(uint32_t) * 2*/
#define HASH_FORMAT "%04X%04X" /* dirname csum, filename csum */

typedef struct uuid_t
  {
    uint64_t id;
    /* auxillary fields */
    uint16_t dname;
    uint16_t fname;
  } uuid_t;

/** API functions */
/** common functions */
int uuid_generate(uuid_t *uuid, const char *path);

/** specific parts of uuid */
/* id */
int uuid_id_validate(char *uuid);
int uuid_id_parse(uuid_t *uuid, char *str);
char *uuid_id_printf(uuid_t *uuid);
