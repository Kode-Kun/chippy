/*
 * chippy : chip-8 emulator
 * Copyright (C) 2026 Lui Sant'Ana Cardoso
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* This little script simply writes raw binary data to a test ROM so we
   can run tests in development to see if each opcode is implemented
   correctly.
   The `rom.c8` file gets overwritten each time this script is called. */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

//utility function to always write in big-endian
uint8_t write_be16(FILE *f, uint16_t v)
{
  uint8_t be[2];
  be[0] = (v >> 8) & 0xFF;
  be[1] = v & 0xFF;
  if(fwrite(be, 1, 2, f) != 2) return 1;
  return 0;
}

int main()
{
  char *filename = "rom.c8";
  FILE *romf     = fopen(filename, "w");
  uint16_t rom[] =  { 0x2010, 0x3168, 0x6030, 0x7001, 0x00E0,
		      0x3069, 0x1006, 0x1014, 0x6268, 0x00EE, 0x00E0, 0x8300, 0x833E, 0x00E0 };
  if(!romf){
    perror("fopen");
    exit(1);
  }

  for(int i = 0; i < ARRAY_SIZE(rom); i++){
    if(write_be16(romf, rom[i]) != 0){
      fprintf(stderr, "ERROR: failed to write rom to %s\n", filename);
      exit(1);
    }
  }

  fclose(romf);
  return 0;
}
