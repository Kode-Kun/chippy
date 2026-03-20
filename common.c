#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "common.h"

// loads ROM file into memory (uint8_t mem[MEM_SIZE])
int load_rom(char *filepath, size_t *filesize, uint8_t *mem)
{
  FILE *src_fd;

  if ((src_fd = fopen(filepath, "r")) == NULL){
    perror(filepath);
    exit(1);
  }

  fseek(src_fd, 0, SEEK_END);
  size_t fsize = ftell(src_fd);
  assert(fsize > 0);
  *filesize = fsize;

  if (fsize > MEM_SIZE){
    fprintf(stderr, "ERROR: File \"%s\" too big to fit in memory (%d).\n",
	   filepath, MEM_SIZE);
    exit(1);
  }

  fseek(src_fd, 0, SEEK_SET);
  fread(&mem[0x200], 1, fsize, src_fd); //account for offset
  printf("Loaded %s (%zu bytes) successfully :3\n", filepath, fsize);

  return 0;
}

uint16_t fetch(uint8_t *mem, uint16_t *PC)
{
  uint16_t op = ((uint16_t)*(mem + *PC) << 8) | ((uint16_t)*(mem + (*PC+1)));
  *PC += 2;

  return op;
}
