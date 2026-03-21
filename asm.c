#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#include "common.h"

int main(int argc, char **argv)
{
  char *input_path = NULL;
  char *rom_path   = NULL;

  int ch;

  while ((ch = getopt(argc, argv, "gf:o:")) != -1) {
    switch (ch) {
    case 'f':
      input_path = optarg;
      break;
    case 'o':
      rom_path = optarg;
      break;
    case '?':
      if(optopt == 'f') fprintf(stderr, "Option -%c requires an argument.\n", optopt);
      else if(isprint(optopt)) fprintf(stderr, "Option -%c is not supported.\n", optopt);
      break;
    default:
      fprintf(stderr, "Usage: chasm -f <file.asm> -o <rom.ch8>\n");
      abort();
    }
  }
  argc -= optind;
  argv += optind;

  if (!rom_path) rom_path = "rom.ch8";
  if (input_path) /*do thing*/;
  else{
    fprintf(stderr, "Info: option '-f' takes an argument.\n");
    exit(1);
  }
  printf("Input argument: %s\n", input_path);
  printf("Rom path: %s\n", rom_path);
  return 0;
}
