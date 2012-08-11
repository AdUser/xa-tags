#define UUID_CHAR_LEN ((sizeof(uint64_t) + 2 * sizeof(uint16_t))* 2) + 2

typedef struct uuid_t
  {
    uint64_t id;
    uint16_t dname;
    uint16_t fname;
  } uuid_t;
