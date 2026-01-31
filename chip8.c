/*
 * C8ke : chip-8 emulator
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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "chip8.h"
#include "font.h"

int main(int argc, char **argv)
{
  char *filepath = NULL;

  int ch;

  while ((ch = getopt(argc, argv, "gf:")) != -1) {
    switch (ch) {
    case 'f':
      filepath = optarg;
      break;
    case '?':
      if(optopt == 'f') fprintf(stderr, "Option -%c requires an argument.\n", optopt);
      else if(isprint(optopt)) fprintf(stderr, "Option -%c is not supported.\n", optopt);
      break;
    default:
      abort();
    }
  }
  argc -= optind;
  argv += optind;

  size_t filesize = 0;
  if(filepath)load_rom(filepath, &filesize);
  else printf("you forgot to provide a source file\n");
  
  return 0;
}

// loads ROM file into memory (uint8_t mem[MEM_SIZE])
int load_rom(char *filepath, size_t *filesize)
{
  FILE *src_fd;

  if ((src_fd = fopen(filepath, "r")) == NULL){
    perror(filepath);
    exit(1);
  }

  fseek(src_fd, 0, SEEK_END);
  size_t fsize = ftell(src_fd);
  *filesize = fsize;

  if (fsize > MEM_SIZE){
    printf("ERROR: File \"%s\" too big to fit in memory (%d). Either recompile c8ke with a bigger MEM_SIZE or provide a smaller ROM.\n",
	   filepath, MEM_SIZE);
    exit(1);
  }

  fseek(src_fd, 0, SEEK_SET);
  fread(&mem, 1, fsize, src_fd);

  printf("Loaded %s (%zu bytes) successfully :3\n", filepath, fsize);
  // code for printing out the memory in hex, might come in handy for debugging later on
  /* printf("%s\n", __line); */
  /* printf("MEMORY:\n"); */
  /* for (int i = 0; i < fsize; i++){ */
  /*   printf("%#X ", mem[i]); */
  /* } */
  /* printf("\n%s\n", __line); */

  return 0;
}
