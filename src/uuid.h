#ifdef UUID64
  #define UUID_CHAR_LEN ((sizeof(uint64_t) + 2 * sizeof(uint16_t))* 2) + 2
  #define UUID_FORMAT "%016llX-%04hX-%04hX"
#else
  #define UUID_CHAR_LEN ((sizeof(uint32_t) + 2 * sizeof(uint16_t))* 2) + 2
  #define UUID_FORMAT "%08X-%04hX-%04hX"
#endif

typedef struct uuid_t
  {
#ifdef UUID64
    uint64_t id;
#else
    uint32_t id;
#endif
    uint16_t dname;
    uint16_t fname;
  } uuid_t;

/** API functions */
int uuid_validate(char *uuid);
int uuid_parse(uuid_t *uuid, char *str);
int uuid_generate(uuid_t *uuid, char *path);
char *uuid_printf(uuid_t *uuid);
