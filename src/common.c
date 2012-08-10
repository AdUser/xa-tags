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

/** hash & checksum functions */
/* -----------------8<------------------------ */
/* Copied from wikipedia */

/*
  Name  : CRC-16 CCITT
  Poly  : 0x1021
  Init  : 0xFFFF
  Revert: false
  XorOut: 0x0000
  Check : 0x29B1 ("123456789")
  MaxLen: 4095 bytes
*/

uint16_t
crc16(char *str, size_t len)
{
  uint16_t crc = 0xFFFF;
  uint8_t i;

  while (len--)
    {
      crc ^= (*str++) << 8;

      for (i = 0; i < 8; i++)
          crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
    }

  return crc;
}

/*
  Name  : Jenkins Hash
*/

uint32_t
jhash(char *key, size_t len)
{
  uint32_t hash, i;

  for (hash = i = 0; i < len; ++i)
    {
      hash += key[i];
      hash += (hash << 10);
      hash ^= (hash >> 6);
    }

  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);

  return hash;
}
/* -----------------8<------------------------ */
