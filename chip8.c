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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>

#include "raylib.h"
#include "font.h"

#define MEM_SIZE          4096
#define FLASH_MAX_SIZE    4096
#define STACK_MAX_SIZE    256
#define SCREEN_WIDTH      64
#define SCREEN_HEIGHT     32
#define __line "--------------------------------"


/* datatypes */
typedef struct{
  uint16_t stack[STACK_MAX_SIZE];
  int top;
} c8_stack_t;

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

/* global variables */
uint8_t  regs[16];
uint8_t  mem[MEM_SIZE];
uint8_t  flash[FLASH_MAX_SIZE];
uint8_t  screen[SCREEN_WIDTH * SCREEN_HEIGHT];
uint16_t I;
uint16_t PC;

/* function protypes */
c8_stack_t stack_init();
uint16_t stack_push(c8_stack_t *stack, uint16_t val);
uint16_t stack_pop(c8_stack_t *stack);
int load_rom(char *filepath, size_t *filesize);
void init_chip();
uint16_t fetch();

int main(int argc, char **argv)
{
  /*   parsing args   */
  char *filepath = NULL;

  int ch;

  while ((ch = getopt(argc, argv, "gf:")) != -1) {
    switch (ch) {
    case 'f':
      filepath = optarg;
      break;
    case '?':
      if(optopt == 'f') fprintf(stderr, "Option -%c requires an argument.\n", optopt);
      else if(isprint(optopt)) fprintf(stderr, "Option -%c is not supported.\n", optopt);
      break;
    default:
      abort();
    }
  }
  argc -= optind;
  argv += optind;

  /*   getting ROM file from args   */
  size_t filesize = 0;
  if(filepath) load_rom(filepath, &filesize);
  else{
    fprintf(stderr, "ERROR: you forgot to provide a source file\n");
    exit(1);
  }

  /*   START OF WINDOWING   */
  InitWindow(SCREEN_WIDTH*10, SCREEN_HEIGHT*10, "chippy");
  SetTargetFPS(60);

  init_chip();
  c8_stack_t s = stack_init();
  bool done_exec = false; //temporary; for debugging only

  while (!WindowShouldClose())
  {
    BeginDrawing();
    ClearBackground(BLACK);

    /*   EXECUTION   */
    if(!done_exec){
      union opcode op = { .raw = fetch() };
      printf("flash[PC]: %#x\nflash[PC+1]: %#x\nopcode: %#x\nPC: %d\n",
	     flash[PC-2], flash[PC-1], op.raw, PC);
      if(PC > filesize) done_exec = true;

      switch(op.base_f.start){
      case 0x0: {
	if(op.raw == 0x00E0){
	  ClearBackground(BLACK);
	}
	else if(op.raw == 0x00EE){
	  uint16_t address = stack_pop(&s);
	  PC = address;
	}
	break;
      }
      case 0x1: {
	uint16_t address = op.ad_f.address;
	PC = address;
	break;
      }
      case 0x2: {
	uint16_t address = op.ad_f.address;
	stack_push(&s, PC);
	PC = address;
	break;
      }
      case 0x3: {
	uint8_t regx = op.base_f.x;
	uint8_t val = op.const_f.const8;
	if(regs[regx] == val) PC += 2;
	break;
      }
      case 0x4: {
	uint8_t regx = op.base_f.x;
	uint8_t val = op.const_f.const8;
	if(regs[regx] != val) PC += 2;
	break;
      }
      case 0x5: {
	uint8_t regx = op.base_f.x;
	uint8_t regy = op.base_f.y;
	if(regs[regx] == regs[regy]) PC += 2;
	break;
      }
      case 0x6: {
	uint8_t regx = op.base_f.x;
	uint8_t val = op.const_f.const8;
	regs[regx] = val;
	break;
      }
      case 0x7: {
	uint8_t regx = op.base_f.x;
	uint8_t val = op.const_f.const8;
	regs[regx] += val;
	break;
      }
      case 0x8: {
	uint8_t regx = op.base_f.x;
	uint8_t regy = op.base_f.y;
	switch(op.base_f.end){
	case 0x0:  //0x8XY0
	  regs[regx] = regs[regy];
	  break;
	case 0x1:  //0x8XY1
	  regs[regx] |= regs[regy];
	  break;
	case 0x2:  //0x8XY2
	  regs[regx] &= regs[regy];
	  break;
	case 0x3:  //0x8XY3
	  regs[regx] ^= regs[regy];
	  break;
	case 0x4:  //0x8XY4
	  regs[regx] += regs[regy];
	  break;
	case 0x5:  //0x8XY5
	  regs[regx] -= regs[regy];
	  break;
	case 0x6:  //0x8XY6
	  regs[regx] = (regs[regx] >> 1);
	  break;
	case 0x7:  //0x8XY7
	  regs[regx] = (regs[regy] - regs[regx]);
	  break;
	case 0xE:  //0x8XYE
	  regs[regx] = (regs[regx] << 1);
	  break;
	default:
	  fprintf(stderr, "ERROR: Opcode %#X is not a valid operation.\n", op.raw);
	  break;
	}
	break;
      } }
    }

    // stack debugging
    if(IsKeyPressed(KEY_P)){
      stack_pop(&s);
    }
    if(IsKeyPressed(KEY_O)){
      stack_push(&s, 0xFFFF);
    }
    if(IsKeyPressed(KEY_R)){
      init_chip();
      done_exec = false;
      s = stack_init();
    }

    // more debugging
    DrawText("Chip8 emulator", 10, 10, 40, RAYWHITE);
    DrawText(TextFormat("Registers: %#X %#X %#X %#X",
			regs[0], regs[1], regs[2], regs[3]), 10, 60, 20, RAYWHITE);
    DrawText(TextFormat("Stack: %#X %#X %#X %#X",
    			 s.stack[0], s.stack[1], s.stack[2], s.stack[3]),
    	      10, 90, 20, RAYWHITE);

    EndDrawing();
  }

  CloseWindow();        // Close window and OpenGL context

  /*   END OF WINDOWING   */

  return 0;
}

// loads ROM file into flash memory (uint8_t flash[FLASH_MAX_SIZE])
int load_rom(char *filepath, size_t *filesize)
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

  if (fsize > FLASH_MAX_SIZE){
    fprintf(stderr, "ERROR: File \"%s\" too big to fit in memory (%d).\n",
	   filepath, FLASH_MAX_SIZE);
    exit(1);
  }

  fseek(src_fd, 0, SEEK_SET);
  fread(&flash, 1, fsize, src_fd);

  printf("Loaded %s (%zu bytes) successfully :3\n", filepath, fsize);
  // code for printing out the memory in hex, might come in handy for debugging later on
  /* printf("%s\n", __line); */
  /* printf("MEMORY:\n"); */
  /* for (int i = 0; i < fsize; i++){ */
  /*   printf("%#X ", flash[i]); */
  /* } */
  /* printf("\n%s\n", __line); */

  return 0;
}

// initialize registers and clocks
void init_chip()
{
  for(int i = 0; i < 16; i++){
    regs[i] = 0x0000;
  }
  I = 0x0000;
  PC = 0x0000;
}

uint16_t fetch()
{
  uint16_t op = ((uint16_t)flash[PC] << 8) | ((uint16_t)flash[PC+1]);
  PC += 2;

  return op;
}

c8_stack_t stack_init()
{
  c8_stack_t stack;
  stack.top = -1;
  return stack;
}

// returns val on success, returns 1 on stack overflow
uint16_t stack_push(c8_stack_t *s, uint16_t val)
{
  if(s->top == (STACK_MAX_SIZE - 1)){
    fprintf(stderr, "ERROR: stack overflow.\n");
    return 0;
  }
  s->stack[s->top + 1] = val;
  s->top++;
  return val;
}

// returns popped value on success, returns 0 when attempting to pop empty stack
uint16_t stack_pop(c8_stack_t *s)
{
  if(s->top == -1){
    fprintf(stderr, "ERROR: attempting to pop empty stack.\n");
    return 0;
  }
  uint16_t ret = s->stack[s->top];
  s->stack[s->top] = 0;
  s->top--;
  return ret;
}
