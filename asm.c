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

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#include "common.h"

struct symb_t {
  char **symbols;
  size_t size;
} symbol_table;

void init_symb(void)
{
  symbol_table.symbols = malloc(sizeof(char**));
  symbol_table.size = 0;
}

void append_symb(char* symb)
{
  size_t symb_len = strlen(symb) + 1;
  symbol_table.symbols[symbol_table.size] = malloc(symb_len);
  strlcpy(symbol_table.symbols[symbol_table.size],
	  symb, symb_len);
  symbol_table.size++;
}

char *get_symb(char *query)
{
  char *sep = ":\n\t\r";
  for(size_t i = 0; i < symbol_table.size; i++){
    // get copy of symbol[i] in symbol table
    int len = strlen(symbol_table.symbols[i]) + 1;
    char *entry = malloc(len);
    strlcpy(entry, symbol_table.symbols[i], len);

    char *symbol = strtok(entry, sep);
    if(strcmp(symbol, query) == 0) return strtok(NULL, sep);

    free(entry);
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

instruction_t lex(char *line, int linenum)
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

  while((data = strtok(l, sep)) != NULL){
    l = NULL;
    token_t tok = nulltok;

    // check for non-instruction tokens
    switch(data[0]){
    case ';':
      tok.type = TokenComment;
      inst.tokens[inst.count++] = tok;
      return inst;
      break;
    case '.':
      tok.type = TokenLabel;
      tok.data = data;
      break;
    case 'V':
    case 'v':
      if(!(data[1] >= '0' && data[1] <= '9') && // if character after V isn't valid
	 !(data[1] >= 'a' && data[1] <= 'f') &&
	 !(data[1] >= 'A' && data[1] <= 'F')){
	tok.type = TokenNull;
	tok.data = REGISTER_ERROR;
	break;
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
	tok.type = TokenNull;
	tok.data = ADDRESS_NOTATION_ERROR;
	break;
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
      tok.data = UNKNOWN_ERROR;
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
  int linenum = 1;
  init_symb();

  while((linelen = getline(&line, &linecap, input_f) != -1)){
    instruction_t inst = lex(line, linenum);
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
    //opcode op = parse(lex(line)); // we parse the instruction returned by lex()
    //if(op.raw == 0x0000) continue;
    //write_be16(rom_f, op.raw)t
    linenum++;
  }

  append_symb(".label1:0x223");
  append_symb(".label2:0x069");

  printf("%s\n", get_symb(".label1"));
  printf("%s\n", get_symb(".label2"));
  char *symbol_1 = get_symb(".label3");
  printf("%s\n", symbol_1 == NULL ? "Error: unkown symbol.\n" : symbol_1);

  printf("Input argument: %s\n", input_path);
  printf("Rom path: %s\n", rom_path);
  return 0;
}
