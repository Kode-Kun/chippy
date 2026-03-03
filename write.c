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

int main()
{
  char *filename = "rom.c8";
  FILE *romf     = fopen(filename, "w");
  uint8_t rom[] =  { 0x61, 0xAE, 0x62, 0x86, 0x63, 0x69,
		     0x00, 0xE0, 0x00, 0xE0, 0x00, 0xE0 };
  if(!romf){
    perror("fopen");
    exit(1);
  }

  if(fwrite(rom, ARRAY_SIZE(rom), sizeof(uint8_t), romf) != (ARRAY_SIZE(rom)*sizeof(uint8_t))){
      fprintf(stderr, "ERROR: failed to write rom to %s\n", filename);
      exit(1);
  }

  fclose(romf);
  return 0;
}
