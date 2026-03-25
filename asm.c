#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <ctype.h>

#include "common.h"

//actually returns instruction_t
instruction_t tokenize(char *line)
{
  ;// TODO
}

opcode parse(instruction_t inst)
{
  ;// TODO
}

void chasm_handle_args(int argc, char **argv, char **input_path, char **rom_path)
{
  int ch;

  while ((ch = getopt(argc, argv, "gf:o:")) != -1) {
    switch (ch) {
    case 'f':
      *input_path = optarg;
      break;
    case 'o':
      *rom_path = optarg;
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

  if (!*rom_path) *rom_path = "rom.ch8";
  if (!*input_path){
    fprintf(stderr, "Info: option '-f' takes an argument.\n");
    exit(1);
  }
}

int main(int argc, char **argv)
{
  char *input_path = NULL;
  char *rom_path   = NULL;

  chasm_handle_args(argc, argv, &input_path, &rom_path);
  FILE *input_f = fopen(input_path, "r");
  if(!input_f){
    perror(input_path);
    exit(1);
  }

  uint8_t mem[MEM_SIZE] = {0};
  char *line = NULL;
  size_t linelen;
  size_t linecap = 0;
  int linenum = 0;

  while((linelen = getline(&line, &linecap, input_f) != -1)){
    opcode op = parse(tokenize(line)); // we parse the instruction returned by tokenize()
    if(op.raw == 0x0000) continue;
    write_be16(rom_path, op.raw);
  }

  printf("Input argument: %s\n", input_path);
  printf("Rom path: %s\n", rom_path);
  return 0;
}
