#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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

char *token_to_str(TokenType t)
{
  char *type;
  switch(t){
  case TokenMov:
    type = "TokenMov";
    break;
  case TokenClear:
    type = "TokenClear";
    break;
  case TokenRet:
    type = "TokenRet";
    break;
  case TokenB:
    type = "TokenB";
    break;
  case TokenCall:
    type = "TokenCall";
    break;
  case TokenIfe:
    type = "TokenIfe";
    break;
  case TokenAdd:
    type = "TokenAdd";
    break;
  case TokenOr:
    type = "TokenOr";
    break;
  case TokenAnd:
    type = "TokenAnd";
    break;
  case TokenXor:
    type = "TokenXor";
    break;
  case TokenSub:
    type = "TokenSub";
    break;
  case TokenLls:
    type = "TokenLls";
    break;
  case TokenSus:
    type = "TokenSus";
    break;
  case TokenLrs:
    type = "TokenLrs";
    break;
  case TokenIfne:
    type = "TokenIfne";
    break;
  case TokenBa:
    type = "TokenBa";
    break;
  case TokenRand:
    type = "TokenRand";
    break;
  case TokenDrw:
    type = "TokenDrw";
    break;
  case TokenIke:
    type = "TokenIke";
    break;
  case TokenIkn:
    type = "TokenIkn";
    break;
  case TokenGdl:
    type = "TokenGdl";
    break;
  case TokenGtk:
    type = "TokenGtk";
    break;
  case TokenSdl:
    type = "TokenSdl";
    break;
  case TokenSsd:
    type = "TokenSsd";
    break;
  case TokenSpr:
    type = "TokenSpr";
    break;
  case TokenBcd:
    type = "TokenBcd";
    break;
  case TokenDpr:
    type = "TokenDpr";
    break;
  case TokenLdr:
    type = "TokenLdr";
    break;
  case TokenReg:
    type = "TokenReg";
    break;
  case TokenConst:
    type = "TokenConst";
    break;
  case TokenAddr:
    type = "TokenAddr";
    break;
  case TokenComment:
    type = "TokenComment";
    break;
  case TokenLabel:
    type = "TokenLabel";
    break;
  case TokenNull:
    type = "TokenNull";
    break;
  case TokenCount:
    type = "TokenCount";
    break;
  default:
    type = "Unknown!";
    break;
  }
  return type;
}
