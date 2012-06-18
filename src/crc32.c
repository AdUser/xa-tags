#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/*
  Name  : CRC-32
  Poly  : 0x04C11DB7    x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 
                       + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1
  Init  : 0xFFFFFFFF
  Revert: true
  XorOut: 0xFFFFFFFF
  Check : 0xCBF43926 ("123456789")
  MaxLen: 268 435 455 байт (2 147 483 647 бит) - обнаружение
   одинарных, двойных, пакетных и всех нечетных ошибок
*/
uint_least32_t crc32(unsigned char *buf, size_t len)
{
    uint_least32_t crc_table[256];
    uint_least32_t crc; int i, j;

    for (i = 0; i < 256; i++)
    {
        crc = i;
        for (j = 0; j < 8; j++)
            crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;

        crc_table[i] = crc;
    };

    crc = 0xFFFFFFFFUL;

    while (len--)
        crc = crc_table[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);

    return crc ^ 0xFFFFFFFFUL;
}

int main(int argc, char **argv)
  {
    char *p;
    char rp[PATH_MAX];
    int fp;
    struct stat st;
    size_t len;
    unsigned char buf[128];
    struct { uint32_t path; uint32_t name; uint32_t uniq; } uuid;

    if (argc < 2) return 1;

    realpath(argv[1], rp);
    printf("'%s' -> ", rp);
    memset(&uuid, 0x0, sizeof(uuid));
    memset(&st,   0x0, sizeof(st));
    len = strlen(rp);
    stat(rp, &st);

    if (S_ISDIR(st.st_mode))
      {
        uuid.path = crc32((unsigned char *) rp, len);
        uuid.name = 0x0;
      }
    else
      {
        p = strrchr(rp, '/');
        uuid.path = crc32((unsigned char *) rp, p - rp);
        uuid.name = crc32((unsigned char *) p, strlen(p));
      }

    fp = open("/dev/urandom", O_RDONLY);
    read(fp, buf, 128);
    uuid.uniq = crc32(buf, 128);
    printf("%08X-%08X-%08X\n", uuid.path, uuid.name, uuid.uniq);

    return 0;
  }
