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
int uuid_validate(char *uuid);
int uuid_parse(uuid_t *uuid, char *str);
int uuid_generate(uuid_t *uuid, const char *path);
char *uuid_printf(uuid_t *uuid);
