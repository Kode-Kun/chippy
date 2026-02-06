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

#ifndef CHIP8_H_
#define CHIP8_H_

#include <stdint.h>

#define MEM_SIZE          4096
#define FLASH_MAX_SIZE    4096
#define STACK_MAX_SIZE    256
#define SCREEN_WIDTH      64
#define SCREEN_HEIGHT     32
#define __line "--------------------------------"

typedef struct{
  uint16_t stack[STACK_MAX_SIZE];
  int top;
} c8_stack_t;

uint8_t  regs[16];
uint8_t  mem[MEM_SIZE];
uint8_t  flash[FLASH_MAX_SIZE];
uint8_t  screen[SCREEN_WIDTH * SCREEN_HEIGHT];
uint16_t I;
uint16_t PC;

/* stack-related functions */
c8_stack_t stack_init();
uint8_t stack_push(c8_stack_t *stack, uint16_t val);
uint8_t stack_pop(c8_stack_t *stack);

int load_rom(char *filepath, size_t *filesize);
void init_chip();
uint16_t fetch();

#endif
