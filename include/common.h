#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MEM_SIZE          4096

typedef enum {
  TokenMov,      // 8XY0, 6XNN, ANNN
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
  TokenComment
} TokenType;

typedef struct {
  TokenType type;
  char *data;
} token_t;

union opcode {
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
};

int load_rom(char *filepath, size_t *filesize, uint8_t *mem);
uint16_t fetch(uint8_t *mem, uint16_t *PC);
uint8_t write_be16(FILE *f, uint16_t v);

#endif
