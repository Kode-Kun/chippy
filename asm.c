#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#include "common.h"

instruction_t tokenize(char *line)
{
  char *l = line;
  char *data;
  instruction_t inst = {
    .count = 0,
  };
  token_t nulltok = {
    .data = NULL,
    .type = TokenNull,
  };
  char *sep = " ,\t\n\r";

  switch(line[0]){
  case ';':
    return (instruction_t) { .count = 1, .tokens = {nulltok} };
    break;
  }

  while((data = strtok(l, sep)) != NULL){
    l = NULL;
    token_t tok = nulltok;
    tok.data = data;

    switch(data[0]){
    case ';':
      return inst;
      break;
    case '.':
      return (instruction_t) {
	.count = 1,
	.tokens = {(token_t){ .data = data, .type = TokenLabel }}
      };
      break;
    case 'V':
    case 'v':
      tok.type = TokenReg;
      break;
    case '#':
      tok.type = TokenConst;
      break;
    case '0':
      if(data[1] != 'x'){
	tok = nulltok;
	break;
      }
      tok.type = TokenAddr;
      break;
    }
    if(tok.type == TokenNull){
      for(int i = 0; i < ARRAY_SIZE(instructions); i++){
	tok = nulltok;

	if(strcasecmp(data, instructions[i]) == 0){
	  tok.data = data;
	  tok.type = i;
	  break;
	}
      }
    }
    inst.tokens[inst.count++] = tok;
  }
  return inst;
}

//opcode parse(instruction_t inst)
//{
//  for(int i = 0; i > inst.count; i++){
//    fprintf(stderr, "Token %d: %s", i, (inst.tokens[i].type != TokenComment) ? instructions[inst.tokens[i].type] : "comment");
//  }
//}

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
  FILE *rom_f = fopen(rom_path, "r");
  if(!input_f){
    perror(input_path);
    exit(1);
  }
  if(!rom_f){
    perror(rom_path);
    exit(1);
  }

  //uint8_t mem[MEM_SIZE] = {0};
  char *line = NULL;
  size_t linelen;
  size_t linecap = 0;
  //int linenum = 0;

  int j = 0;
  while((linelen = getline(&line, &linecap, input_f) != -1)){
    instruction_t inst = tokenize(line);
    fprintf(stderr, "Instruction %d\n", j);
    for(int i = 0; i < inst.count; i++){
      fprintf(stderr, "Token %d: %s of type %s\n", i, inst.tokens[i].data, token_to_str(inst.tokens[i].type));
    }
    //opcode op = parse(tokenize(line)); // we parse the instruction returned by tokenize()
    //if(op.raw == 0x0000) continue;
    //write_be16(rom_f, op.raw);
    j++;
  }

  printf("Input argument: %s\n", input_path);
  printf("Rom path: %s\n", rom_path);
  return 0;
}
