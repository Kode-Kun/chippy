/*
 * chippy, chasm: chip-8 emulator and assembler
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
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>

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
  fclose(src_fd);

  return 0;
}

uint16_t fetch(uint8_t *mem, uint16_t *PC)
{
  uint16_t op = ((uint16_t)*(mem + *PC) << 8) | ((uint16_t)*(mem + (*PC+1)));
  *PC += 2;

  return op;
}

//utility function to always write in big-endian
uint8_t write_be16(FILE *f, uint16_t v)
{
  uint8_t be[2];
  be[0] = (v >> 8) & 0xFF;
  be[1] = v & 0xFF;
  if(fwrite(be, 1, 2, f) != 2) return 1;
  return 0;
}

bool is_empty_line(char *l)
{
  bool b = true;
  for(int i = 0; l[i] != '\0'; i++){
    if(!isspace(l[i])) b = false;
  }
  return b;
}


char *token_to_str(token_kind_t k)
{
  char *kind;
  switch(k){
  case TokenMov:
    kind = "TokenMov";
    break;
  case TokenClear:
    kind = "TokenClear";
    break;
  case TokenRet:
    kind = "TokenRet";
    break;
  case TokenB:
    kind = "TokenB";
    break;
  case TokenCall:
    kind = "TokenCall";
    break;
  case TokenIfe:
    kind = "TokenIfe";
    break;
  case TokenAdd:
    kind = "TokenAdd";
    break;
  case TokenOr:
    kind = "TokenOr";
    break;
  case TokenAnd:
    kind = "TokenAnd";
    break;
  case TokenXor:
    kind = "TokenXor";
    break;
  case TokenSub:
    kind = "TokenSub";
    break;
  case TokenLls:
    kind = "TokenLls";
    break;
  case TokenSus:
    kind = "TokenSus";
    break;
  case TokenLrs:
    kind = "TokenLrs";
    break;
  case TokenIfne:
    kind = "TokenIfne";
    break;
  case TokenBa:
    kind = "TokenBa";
    break;
  case TokenRand:
    kind = "TokenRand";
    break;
  case TokenDrw:
    kind = "TokenDrw";
    break;
  case TokenIke:
    kind = "TokenIke";
    break;
  case TokenIkn:
    kind = "TokenIkn";
    break;
  case TokenGdl:
    kind = "TokenGdl";
    break;
  case TokenGtk:
    kind = "TokenGtk";
    break;
  case TokenSdl:
    kind = "TokenSdl";
    break;
  case TokenSsd:
    kind = "TokenSsd";
    break;
  case TokenSpr:
    kind = "TokenSpr";
    break;
  case TokenBcd:
    kind = "TokenBcd";
    break;
  case TokenDpr:
    kind = "TokenDpr";
    break;
  case TokenLdr:
    kind = "TokenLdr";
    break;
  case TokenReg:
    kind = "TokenReg";
    break;
  case TokenConst:
    kind = "TokenConst";
    break;
  case TokenAddr:
    kind = "TokenAddr";
    break;
  case TokenComment:
    kind = "TokenComment";
    break;
  case TokenLabel:
    kind = "TokenLabel";
    break;
  case TokenNull:
    kind = "TokenNull";
    break;
  case TokenCount:
    kind = "TokenCount";
    break;
  default:
    kind = "Unknown!";
    break;
  }
  return kind;
}
