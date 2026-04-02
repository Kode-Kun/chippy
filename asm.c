/*
 * chasm : chip-8 assembler
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
#include <stdbool.h>

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

void append_symbol(char* symb)
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

char *get_symbol(char *query)
{
  int len = strlen(query);
  for(size_t i = 0; i < symbol_table.size; i++){
    char *current = symbol_table.symbols[i];
    if(strncmp(current, query, len) == 0 && current[len] == ':'){
      return current + len + 1;
    }
  }
  return NULL;
}

uint16_t str_to_hex(char *str, int base)
{
  uint16_t hex;
  char *endptr;
  long int decimal = strtol(str, &endptr, base);

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
  ssize_t linelen;
  size_t linecap = 0;
  int address = 0x200;
  int linenum = 1;

  while((linelen = getline(&line, &linecap, src_fd)) != -1){
    if(line[0] == '.'){
      char *sep = " ,:\t\n\r";
      char *l = malloc(LABEL_MAX_SIZE + 1);
      strncpy(l, line, LABEL_MAX_SIZE + 1);
      char *label = strtok(l, sep);
      if(get_symbol(label) != NULL){ // if label is defined more than once
	fprintf(stderr, ERROR_FORMAT, filepath, linenum, 0, LABEL_ERROR);
	exit(1);
      }
      char *symbol = malloc(LABEL_MAX_SIZE + 7); // 7 to account for the :0xNNN that goes after the label
      snprintf(symbol, LABEL_MAX_SIZE + 7, "%s:%#X", label, address);
      append_symbol(symbol);
      free(l);
      free(symbol);
      linenum++;
      continue;
    }
    if(line[0] != ';' && !is_empty_line(line)) address += 2;
    linenum++;
  }
  free(line);
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
    .kind = TokenNull,
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
      tok.kind = TokenComment;
      inst.tokens[inst.count++] = tok;
      return inst;
      break;
    case '.':
      if(inst.count == 0){
	tok.kind = TokenLabel;
	tok.data = data;
      } else{
	tok.kind = TokenAddr;
	char *addr = get_symbol(data);
	if(addr == NULL){  // if label being called doesn't exist in symbol table
	  fprintf(stderr, ERROR_FORMAT, filepath, tok.line, tok.col, LABEL_UNKNOWN_ERROR);
	  exit(1);
	}
	tok.data = get_symbol(data);
      }
      break;
    case 'V':
    case 'v':
      if(!(data[1] >= '0' && data[1] <= '9') && // if character after V isn't valid
	 !(data[1] >= 'a' && data[1] <= 'f') &&
	 !(data[1] >= 'A' && data[1] <= 'F')){
	fprintf(stderr, ERROR_FORMAT, filepath, tok.line, tok.col, REGISTER_ERROR);
	exit(1);
      }
      tok.kind = TokenReg;
      tok.data = data;
      break;
    case 'I':
      if(strcmp(data, "I") != 0) break;
      tok.kind = TokenReg;
      tok.data = data;
      break;
    case '#':
      if(str_to_hex(data, 0) > 0xFF){ // if data provided can't be represented in 8 bits
	fprintf(stderr, ERROR_FORMAT, filepath, tok.line, tok.col, CONSTANT8_SIZE_ERROR);
	exit(1);
      }
      tok.kind = TokenConst;
      tok.data = data;
      break;
    case '0':
      if(data[1] != 'x' && data[1] != 'X' && // if address is provided in a format that isn't hex or binary
	 data[1] != 'b' && data[1] != 'B'){
	fprintf(stderr, ERROR_FORMAT, filepath, tok.line, tok.col, ADDRESS_NOTATION_ERROR);
	exit(1);
      }
      if(str_to_hex(data, 0) > 0xFFF){ // if address goes out of bounds
	fprintf(stderr, ERROR_FORMAT, filepath, tok.line, tok.col, ADDRESS_BOUNDS_ERROR);
	exit(1);
      }
      tok.kind = TokenAddr;
      tok.data = data;
      break;
    default:
      break;
    }

    // check for instruction tokens
    if(tok.kind == TokenNull && tok.data == NULL){
      for(int i = 0; i < ARRAY_SIZE(instructions); i++){
	tok = nulltok;
	if(strcasecmp(data, instructions[i]) == 0){
	  tok.kind = i;
	  tok.data = data;
	  break;
	}
      }
    }

    if(tok.kind == TokenNull && tok.data == NULL){ // if we still didn't get a match, return unkown error
      fprintf(stderr, ERROR_FORMAT, filepath, tok.line, tok.col, UNKNOWN_ERROR); // if we encounter an invalid token altogether
      exit(1);
    }

    if(inst.count >= 5){ // if instruction has too many tokens (max is 4, +1 for comment)
      fprintf(stderr, ERROR_FORMAT, filepath, linenum, 0, TOO_MANY_TOKS_ERROR);
      exit(1);
    }
    inst.tokens[inst.count++] = tok;
  }
  return inst;
}

opcode_t generate(instruction_t i, char* filename)
{
  opcode_t op;

  token_kind_t tok_kind = i.tokens[0].kind;
  switch(tok_kind){
  case TokenMov:
    // MOV I, NNN
    if(strncmp(i.tokens[1].data, "I", 1) == 0 &&
       i.tokens[2].kind == TokenAddr){
      op.start = 0xA;
      op.address = (str_to_hex(i.tokens[2].data, 0) & 0xFFF);
      break;
    }
    // MOV Vx, #NN
    if(i.tokens[1].kind == TokenReg &&
       i.tokens[2].kind == TokenConst){
      op.start = 0x6;
      op.x = (str_to_hex(&i.tokens[1].data[1], 16) & 0xF);
      op.const8 = (str_to_hex(&i.tokens[2].data[1], 0) & 0xFF);
      break;
    }
    // MOV Vx, Vy
    if(i.tokens[1].kind == TokenReg &&
       i.tokens[2].kind == TokenReg){
      op.start = 0x8;
      op.x = (str_to_hex(&i.tokens[1].data[1], 16) & 0xF);
      op.y = (str_to_hex(&i.tokens[2].data[1], 16) & 0xF);
      op.end = 0x0;
      break;
    }
    else{ // invalid operation structure
      fprintf(stderr, ERROR_FORMAT, filename, i.tokens[1].line, i.tokens[1].col, INVALID_COMP_ERROR);
      exit(1);
    }
  case TokenAdd:
    if(strncmp(i.tokens[1].data, "I", 1) == 0 &&
       i.tokens[2].kind == TokenReg){
      op.start = 0xF;
      op.x = (str_to_hex(&i.tokens[2].data[1], 16) & 0xFFF);
      op.const8 = 0x1E;
      break;
    }
    // ADD Vx, #NN
    if(i.tokens[1].kind == TokenReg &&
       i.tokens[2].kind == TokenConst){
      op.start = 0x7;
      op.x = (str_to_hex(&i.tokens[1].data[1], 16) & 0xF);
      op.const8 = (str_to_hex(&i.tokens[2].data[1], 0) & 0xFF);
      break;
    }
    // ADD Vx, Vy
    if(i.tokens[1].kind == TokenReg &&
       i.tokens[2].kind == TokenReg){
      op.start = 0x8;
      op.x = (str_to_hex(&i.tokens[1].data[1], 16) & 0xF);
      op.y = (str_to_hex(&i.tokens[2].data[1], 16) & 0xF);
      op.end = 0x4;
      break;
    }
    else{ // invalid operation structure
      fprintf(stderr, ERROR_FORMAT, filename, i.tokens[1].line, i.tokens[1].col, INVALID_COMP_ERROR);
      exit(1);
    }
  case TokenIfe:
  case TokenIfne:
    // IFE Vx, #NN
    // IFNE Vx, #NN
    if(i.tokens[1].kind == TokenReg &&
       i.tokens[2].kind == TokenConst){
      if(tok_kind == TokenIfe)  op.start = 0x3;
      if(tok_kind == TokenIfne) op.start = 0x4;
      op.x = (str_to_hex(&i.tokens[1].data[1], 16) & 0xF);
      op.const8 = (str_to_hex(&i.tokens[2].data[1], 0) & 0xFF);
      break;
    }
    // IFE Vx, Vy
    // IFNE Vx, Vy
    if(i.tokens[1].kind == TokenReg &&
       i.tokens[2].kind == TokenReg){
      if(tok_kind == TokenIfe)  op.start = 0x5;
      if(tok_kind == TokenIfne) op.start = 0x9;
      op.x = (str_to_hex(&i.tokens[1].data[1], 16) & 0xF);
      op.y = (str_to_hex(&i.tokens[2].data[1], 16) & 0xF);
      op.end = 0x0;
      break;
    }
  case TokenB:
  case TokenBa:
  case TokenCall:
    if(i.tokens[1].kind != TokenAddr){ // These instructions only take an address as argument
      fprintf(stderr, ERROR_FORMAT, filename, i.tokens[1].line, i.tokens[1].col, INVALID_COMP_ERROR);
      exit(1);
    }
    if(tok_kind == TokenB)    op.start = 0x1;
    if(tok_kind == TokenBa)   op.start = 0xB;
    if(tok_kind == TokenCall) op.start = 0x2;
    op.address = (str_to_hex(i.tokens[1].data, 0) & 0xFFF);
    break;
  case TokenRet:
    op.raw = 0x00EE;
    break;
  case TokenClear:
    op.raw = 0x00E0;
    break;
  }
  return op;
}

void chasm_handle_args(int argc, char **argv, char **input_path, char **rom_path, bool *debug)
{
  int ch;

  while ((ch = getopt(argc, argv, "gi:o:")) != -1) {
    switch (ch) {
    case 'i':
      *input_path = optarg;
      break;
    case 'o':
      *rom_path = optarg;
      break;
    case 'g':
      *debug = true;
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

  bool debug = false;
  chasm_handle_args(argc, argv, &input_path, &rom_path, &debug);
  parse_labels(input_path);

  FILE *input_f = fopen(input_path, "r");
  FILE *rom_f = fopen(rom_path, "w");
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
  int linenum = 0;

  size_t bytes = 0;

  while((linelen = getline(&line, &linecap, input_f)) != -1){
    linenum++;
    if(strncmp(line, "\n", 1) == 0 || strncmp(line, "\r\n", 2) == 0) continue; //skip newlines
    instruction_t inst = lex(line, input_path, linenum);
    if(inst.tokens[0].kind == TokenLabel ||
       inst.tokens[0].kind == TokenComment) continue;
    opcode_t op = generate(inst, input_path);
    if(debug){
      fprintf(stderr, "Instruction %d {\n", linenum);
      for(int i = 0; i < inst.count; i++){
	token_t tok = inst.tokens[i];

	fprintf(stderr, "  Token %d: %s of kind %s\n", i+1, tok.data,
		token_to_str(tok.kind));
	if(tok.kind == TokenAddr || tok.kind == TokenConst){
	  fprintf(stderr, "Value of %s: %d.\n", tok.data, tok.kind == TokenConst ? str_to_hex(&tok.data[1], 0) : str_to_hex(tok.data, 0));
	}
      }
      fprintf(stderr, "Opcode: %#X\n", op);
      fprintf(stderr, "}\n");
    }
    write_be16(rom_f, op.raw);
    bytes += 2;
  }

  printf("%s:0:0: info: Succesfully wrote %zu bytes to %s.\n", input_path, bytes, rom_path);
  free(line);

  if(debug){
    printf("Input argument: %s\n", input_path);
    printf("Rom path: %s\n", rom_path);
  }

  free_symb();
  return 0;
}
