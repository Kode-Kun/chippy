/*
 * chippy : chip-8 emulator, assembler and disassembler
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
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#include "common.h"

#define LABEL_MAX_SIZE 25

struct symb_t {
  char **symbols;
  size_t size;
} symbol_table;

void init_symb(void)
{
  symbol_table.symbols = NULL;
  symbol_table.size = 0;
}

void free_symb(void)
{
  for(size_t i = 0; i < symbol_table.size; i++){
    free(symbol_table.symbols[i]);
  }
  free(symbol_table.symbols);
  symbol_table.size = 0;
}

void append_symb(char* symb)
{
  symbol_table.symbols = realloc(symbol_table.symbols, (symbol_table.size + 1) * sizeof(char*));

  if (symbol_table.symbols == NULL) {
    perror("Failed to allocate symbol table");
    exit(1);
  }

  size_t symb_len = strlen(symb) + 1;
  symbol_table.symbols[symbol_table.size] = malloc(symb_len);
  strncpy(symbol_table.symbols[symbol_table.size],
	  symb, symb_len);
  symbol_table.size++;
}

char *get_symb(char *query)
{
  int len = strlen(query);
  for(size_t i = 0; i < symbol_table.size; i++){
    char *current = symbol_table.symbols[i];
    if(strncmp(current, query, len) == 0){
      return current + len + 1;
    }
  }
  return NULL;
}

uint16_t str_to_hex(char *str)
{
  uint16_t hex;
  char *endptr;
  long int decimal = strtol(str, &endptr, 0);

  // bounds-checking: constants and addresses can't go above 0xFFF
  if(decimal > 0xFFF){
    str = NULL;
  }

  hex = (uint16_t) decimal;
  return hex;
}

void parse_labels(char *filepath)
{
  FILE *src_fd;

  if ((src_fd = fopen(filepath, "r")) == NULL){
    perror(filepath);
    exit(1);
  }

  char *line = NULL;
  size_t linelen;
  size_t linecap = 0;
  int address = 0x200;
  int linenum = 1;

  while((linelen = getline(&line, &linecap, src_fd) != -1)){
    if(line[0] == '.'){
      char *sep = " ,:\t\n\r";
      char *l = malloc(LABEL_MAX_SIZE + 1);
      strncpy(l, line, LABEL_MAX_SIZE + 1);
      char *label = strtok(l, sep);
      if(get_symb(label) != NULL){
	fprintf(stderr, "%s:%d:%d: %s", filepath, linenum, 0, LABEL_ERROR);
	exit(1);
      }
      char *symbol = malloc(LABEL_MAX_SIZE + 7); // 7 to account for the :0xNNN that goes after the label
      snprintf(symbol, LABEL_MAX_SIZE + 7, "%s:%#X", label, address);
      append_symb(symbol);
      free(l);
      free(symbol);
      linenum++;
      continue;
    }
    if(line[0] != ';') address += 2;
    linenum++;
  }
  fclose(src_fd);
}

instruction_t lex(char *line, char *filepath, int linenum)
{
  char *l = line;
  char *data;
  instruction_t inst = {
    .count = 0,
  };
  token_t nulltok = {
    .data = NULL,
    .type = TokenNull,
    .line = linenum,
  };
  char *sep = " ,:\t\n\r";

  while((data = strtok(l, sep)) != NULL){
    l = NULL;
    token_t tok = nulltok;
    tok.col = (int)(data - line);

    // check for non-instruction tokens
    switch(data[0]){
    case ';':
      tok.type = TokenComment;
      inst.tokens[inst.count++] = tok;
      return inst;
      break;
    case '.':
      if(inst.count == 0){
	tok.type = TokenLabel;
	tok.data = data;
      } else{
	tok.type = TokenAddr;
	char *addr = get_symb(data);
	if(addr == NULL){
	  fprintf(stderr, "%s:%d:%d: %s", filepath, tok.line, tok.col, LABEL_UNKOWN_ERROR);
	  exit(1);
	}
	tok.data = get_symb(data);
      }
      break;
    case 'V':
    case 'v':
      if(!(data[1] >= '0' && data[1] <= '9') && // if character after V isn't valid
	 !(data[1] >= 'a' && data[1] <= 'f') &&
	 !(data[1] >= 'A' && data[1] <= 'F')){
	fprintf(stderr, "%s:%d:%d: %s", filepath, tok.line, tok.col, REGISTER_ERROR);
	exit(1);
      }
      tok.type = TokenReg;
      tok.data = data;
      break;
    case 'I':
      tok.type = TokenReg;
      tok.data = data;
      break;
    case '#':
      tok.type = TokenConst;
      tok.data = data;
      break;
    case '0':
      if(data[1] != 'x' && data[1] != 'X' &&
	 data[1] != 'b' && data[1] != 'B'){
	fprintf(stderr, "%s:%d:%d: %s", filepath, tok.line, tok.col, ADDRESS_NOTATION_ERROR);
	exit(1);
      }
      tok.type = TokenAddr;
      tok.data = data;
      break;
    default:
      break;
    }

    // check for instruction tokens
    if(tok.type == TokenNull && tok.data == NULL){
      for(int i = 0; i < ARRAY_SIZE(instructions); i++){
	tok = nulltok;
	if(strcasecmp(data, instructions[i]) == 0){
	  tok.type = i;
	  tok.data = data;
	  break;
	}
      }
    }

    // if we still didn't get a match, return unkown error
    if(tok.type == TokenNull && tok.data == NULL){
      fprintf(stderr, "%s:%d:%d: %s", filepath, tok.line, tok.col, UNKNOWN_ERROR);
      exit(1);
    }

    inst.tokens[inst.count++] = tok;
  }
  return inst;
}

opcode generate(instruction_t inst)
{
  // TODO
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
  init_symb();

  chasm_handle_args(argc, argv, &input_path, &rom_path);
  parse_labels(input_path);

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
  int linenum = 1;

  while((linelen = getline(&line, &linecap, input_f) != -1)){
    instruction_t inst = lex(line, input_path, linenum);
    fprintf(stderr, "Instruction %d {\n", linenum);
    for(int i = 0; i < inst.count; i++){
      token_t tok = inst.tokens[i];

      fprintf(stderr, "  Token %d: %s of type %s\n", i+1, tok.data,
	      token_to_str(tok.type));
      if(tok.type == TokenAddr || tok.type == TokenConst){
	fprintf(stderr, "Value of %s: %d.\n", tok.data, tok.type == TokenConst ? str_to_hex(&tok.data[1]) : str_to_hex(tok.data));
      }
    }
    fprintf(stderr, "}\n");
    linenum++;
  }

  free(line);

  append_symb(".label1:0x223");
  append_symb(".label2:0x069");

  printf("%s\n", get_symb(".label1"));
  printf("%s\n", get_symb(".label2"));
  printf("%s\n", get_symb(".test"));

  printf("Input argument: %s\n", input_path);
  printf("Rom path: %s\n", rom_path);

  free_symb();
  return 0;
}
