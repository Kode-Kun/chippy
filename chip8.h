#ifndef CHIP8_H_
#define CHIP8_H_

#include <stdint.h>

#define MEM_SIZE      4096
#define STACK_SIZE    32
#define SCREEN_WIDTH  64
#define SCREEN_HEIGHT 32

uint8_t  regs[16];
uint8_t  mem[MEM_SIZE];
uint8_t  screen[SCREEN_WIDTH * SCREEN_HEIGHT];
uint16_t stack[STACK_SIZE];
uint16_t I = 0x0000;
uint16_t opcode;

int load_rom(char *filepath);
void init_cpu();
void execute();

#endif
