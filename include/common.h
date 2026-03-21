#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MEM_SIZE          4096

union opcode{
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
