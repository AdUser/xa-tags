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

#define CRC24_INIT 0x00B704CEUL
#define CRC24_POLY 0x01864CFBUL

/* source: http://www.ietf.org/rfc/rfc2440.txt */
/* -----------------8<------------------------ */
typedef uint32_t crc24_t;

crc24_t
crc24(unsigned char *str, size_t len)
{
    crc24_t crc = CRC24_INIT;
    int i;

    while (len--) {
        crc ^= (*str++) << 16;
        for (i = 0; i < 8; i++) {
            crc <<= 1;
            if (crc & 0x1000000)
                crc ^= CRC24_POLY;
        }
    }
    return crc & 0xFFFFFFL;
}
/* -----------------8<------------------------ */

int main(int argc, char **argv)
  {
    char *p;
    char rp[PATH_MAX];
    int fp;
    struct stat st;
    size_t len;
    unsigned char buf[128];
    struct { crc24_t path; crc24_t name; crc24_t uniq; } uuid;

    if (argc < 2) return 1;

    realpath(argv[1], rp);
    printf("'%s' -> ", rp);
    memset(&uuid, 0x0, sizeof(uuid));
    memset(&st,   0x0, sizeof(st));
    len = strlen(rp);
    stat(rp, &st);

    if (S_ISDIR(st.st_mode))
      {
        uuid.path = crc24((unsigned char *) rp, len);
        uuid.name = 0x0;
      }
    else
      {
        p = strrchr(rp, '/');
        uuid.path = crc24((unsigned char *) rp, p - rp);
        uuid.name = crc24((unsigned char *) p, strlen(p));
      }

    fp = open("/dev/urandom", O_RDONLY);
    read(fp, buf, 128);
    uuid.uniq = crc24(buf, 128);
    printf("%06X-%06X-%06X\n", uuid.path, uuid.name, uuid.uniq);

    return 0;
  }

