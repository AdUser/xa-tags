/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#include "common.h"

/** return values:
 * 0 - all ok
 * 1 - error
 */
int
uuid_validate(char *uuid)
{
  int i = 0;
  char c = '\0';
#ifdef UUID64
  const uint8_t pos1 = 16;
  const uint8_t pos2 = 21;
  const uint8_t max  = 26;
#else
  const uint8_t pos1 =  8;
  const uint8_t pos2 = 13;
  const uint8_t max  = 18;
#endif

  ASSERT(uuid != NULL, MSG_M_NULLPTR);

  for (i = 0; i < max; i++)
    {
      c = (isxdigit(uuid[i])) ? '0' : uuid[i];
      switch (c)
        {
          case '0' : if (i == pos1 || i == pos2) return 1; break;
          case '-' : if (i != pos1 && i != pos2) return 1; break;
          default  : /* and '\0' also */         return 1; break;
        }
    }

  return 0;
}

/** return values:
 * 0 - all ok
 * 1 - error
 */
inline int
uuid_parse(uuid_t *uuid, char *str)
{
  ASSERT(uuid != NULL && str != NULL, MSG_M_NULLPTR);

  if (sscanf(str, UUID_FORMAT, &uuid->id, &uuid->dname, &uuid->fname) == 3)
    return 0;

  return 1;
}

/** returns allocated string or NULL on error */
char *
uuid_printf(uuid_t *uuid)
{
  size_t len = UUID_CHAR_LEN + 1;
  char *p = NULL;

  ASSERT(uuid != NULL, MSG_M_NULLPTR);

  CALLOC(p, len, sizeof(char));

#ifdef UUID64
  snprintf(p, len, "%08X%08X-%04X-%04X",
              (uint32_t) (uuid->id >> 32),
              (uint32_t) (uuid->id & 0xFFFFFFFF),
              uuid->dname, uuid->fname);
#else
  snprintf(p, len, "%08X-%04X-%04X",
              uuid->id, uuid->dname, uuid->fname);
#endif

  return p;
}
