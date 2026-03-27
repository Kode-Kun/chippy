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

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MEM_SIZE          3584

#define REGISTER_ERROR         "Error: Invalid register format. Expected V0-VF.\n"
#define CONSTANT8_SIZE_ERROR   "Error: Invalid constant. Expected #NN (0-255).\n"
#define CONSTANT4_SIZE_ERROR   "Error: Invalid constant. Expected #N (0-15).\n"
#define ADDRESS_BOUNDS_ERROR   "Error: Invalid address. Expected 0x200-0x7FF.\n"
#define ADDRESS_NOTATION_ERROR "Error: Invalid address format. Expected 0xNNN.\n"
#define LABEL_ERROR            "Error: Label name collision.\n"
#define UNKNOWN_ERROR          "Error: Unkown token.\n"

#define ARRAY_SIZE(arr) (int)(sizeof(arr) / sizeof((arr)[0]))

typedef enum {
  TokenMov = 0,      // 8XY0, 6XNN, ANNN
  TokenClear,    // 00E0
  TokenRet,      // 00EE
  TokenB,        // 1NNN
  TokenCall,     // 2NNN
  TokenIfe,      // 5XY0
  TokenAdd,      // 7XNN, 8XY4, FX1E
  TokenOr,       // 8XY1
  TokenAnd,      // 8XY2
  TokenXor,      // 8XY3
  TokenSub,      // 8XY5
  TokenLls,      // 8XY6
  TokenSus,      // 8XY7
  TokenLrs,      // 8XYE
  TokenIfne,     // 9XY0
  TokenBa,       // BNNN
  TokenRand,     // CXNN
  TokenDrw,      // DXYN
  TokenIke,      // EX9E
  TokenIkn,      // EXA1
  TokenGdl,      // FX07
  TokenGtk,      // FX0A
  TokenSdl,      // FX15
  TokenSsd,      // FX18
  TokenSpr,      // FX29
  TokenBcd,      // FX33
  TokenDpr,      // FX55
  TokenLdr,      // FX65
  TokenReg,
  TokenConst,
  TokenAddr,
  TokenComment,
  TokenLabel,
  TokenNull,
  TokenCount
} TokenType;

static const char *instructions[] = {
  "MOV",
  "CLEAR",
  "RET",
  "B",
  "CALL",
  "IFE",
  "ADD",
  "OR",
  "AND",
  "XOR",
  "SUB",
  "LLS",
  "SUS",
  "LRS",
  "IFNE",
  "BA",
  "RAND",
  "DRW",
  "IKE",
  "IKN",
  "GDL",
  "GTK",
  "SDL",
  "SSD",
  "SPR",
  "BCD",
  "DPR",
  "LDR",
};

typedef struct {
  TokenType type;
  char *data;
} token_t;

typedef struct {
  int count;
  token_t tokens[5];
} instruction_t;

typedef union {
  struct __attribute__((packed)){
    uint16_t end: 4;
    uint16_t y: 4;
    uint16_t x: 4;
    uint16_t start: 4;
  } base_f;
  struct __attribute__((packed)){
    uint16_t address: 12;
    uint16_t start: 4;
  } ad_f;
  struct __attribute__((packed)){
    uint16_t const8: 8;
    uint16_t x: 4;
    uint16_t start: 4;
  } const_f;
  uint16_t   raw;
} opcode;

int load_rom(char *filepath, size_t *filesize, uint8_t *mem);
uint16_t fetch(uint8_t *mem, uint16_t *PC);
uint8_t write_be16(FILE *f, uint16_t v);
char *token_to_str(TokenType t);

#endif
