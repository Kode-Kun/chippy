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
   The `rom.ch8` file gets overwritten each time this script is called. */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "common.h"

#define ARRAY_SIZE(arr) (int)(sizeof(arr) / sizeof((arr)[0]))

int main()
{
  char *filename = "rom.ch8";
  FILE *romf     = fopen(filename, "w");
  uint16_t rom[] =  { 0x2216, 0x3168, 0x6030, 0x7001, 0x00E0,
		      0x3069, 0x1206, 0x8300, 0x833E, 0xC169,
		      0x121A, 0x6268, 0x00EE, 0x00E0, 0x00E0 };
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

  printf("Info: Succesfully wrote %d bytes to %s.\n", (ARRAY_SIZE(rom) * 2), filename);
  fclose(romf);
  return 0;
}
