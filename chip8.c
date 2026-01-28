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
      if(optopt == 'f') fprintf((FILE*)STDERR_FILENO, "Option -%c requires an argument.\n", optopt);
      else if(isprint(optopt)) fprintf((FILE*)STDERR_FILENO, "Option -%c is not supported.\n", optopt);
      break;
    default:
      abort();
    }
  }
  argc -= optind;
  argv += optind;

  if(filepath)load_rom(filepath);
  else printf("you forgot to provide a source file\n";
  
  return 0;
}

// loads ROM file into memory (uint8_t mem[MEM_SIZE])
int load_rom(char *filepath)
{
  FILE *src_fd;

  if ((src_fd = fopen(filepath, "r")) == NULL){
    fprintf((FILE*)STDERR_FILENO, "%s", strerror(errno));
    return 1;
  }
  printf("Loaded %s successfully :3\n", filepath);
  // actually load the rom into memory here :3

  return 0;
}
