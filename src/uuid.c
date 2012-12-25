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
uuid_generate(uuid_t *uuid, const char *path)
{
  ASSERT(uuid != NULL && path != NULL, MSG_M_NULLPTR);

  uuid->id = 0; /* generated automatically after insert */

  if (strrchr(path, '/') == NULL)
    return 1; /* path MUST contain at least one slash */

  get_path_checksums(uuid, path);

  return 0;
}

/** return values:
 * 0 - all ok
 * 1 - error
 */
int
uuid_id_validate(char *uuid)
{
  int i = 0;
  char c = '\0';

  ASSERT(uuid != NULL, MSG_M_NULLPTR);

  for (i = 0; i < UUID_CHAR_LEN; i++)
    if (!isxdigit(uuid[i]))
      return 1;

  c = uuid[UUID_CHAR_LEN]; /* char right behind uuid */
  if (!(isblank(c) || c == '\0'))
    return 1;

  return 0;
}

/** return values:
 * 0 - all ok
 * 1 - error
 */
inline int
uuid_id_parse(uuid_t *uuid, char *str)
{
  uint8_t i = 0;
  ASSERT(uuid != NULL && str != NULL, MSG_M_NULLPTR);

  if (uuid_id_validate(str))
    return 1;

  uuid->id = 0;
  for (i = 0; i < UUID_CHAR_LEN; i++)
    {
      uuid->id <<= 4;
      uuid->id +=  (isdigit(str[i])) ? str[i] - '0' : str[i] - 'A' + 0xA;
    }

  return 0;
}

char *
uuid_id_printf(const uuid_t *uuid)
{
  static char buf[UUID_CHAR_LEN + 1];

  ASSERT(uuid != NULL, MSG_M_NULLPTR);

  snprintf(buf, UUID_CHAR_LEN + 1, UUID_FORMAT,
              (uint32_t) (uuid->id >> 32),
              (uint32_t) (uuid->id & 0xFFFFFFFF));

  return buf;
}
