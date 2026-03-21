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

/* Chippy is a simple chip-8 emulator written completely in C with raylib.
 * It implements standard CHIP-8, not SCHIP or anything of the sort.
 * Chippy comes with a small assembler, inspired by Christian Egeberg's [Chipper](https://www.hpcalc.org/details/6735).
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>

#include "raylib.h"
#include "font.h"
#include "common.h"

#define STACK_MAX_SIZE    256
#define SCREEN_WIDTH      64
#define SCREEN_HEIGHT     32
#define __line "--------------------------------"

typedef struct{
  uint16_t stack[STACK_MAX_SIZE];
  int top;
} c8_stack_t;

/* function protypes */
c8_stack_t stack_init();
uint16_t stack_push(c8_stack_t *s, uint16_t val);
uint16_t stack_pop(c8_stack_t *s);
void init_chip(uint8_t *regs, uint16_t *I, uint16_t *PC);

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
      fprintf(stderr, "Usage: chippy -f <rom.ch8>\n");
      abort();
    }
  }
  argc -= optind;
  argv += optind;

  /*   initializing the emulator   */
  uint8_t  regs[16];
  uint8_t  mem[MEM_SIZE];
  //uint8_t  screen[SCREEN_WIDTH * SCREEN_HEIGHT];
  uint16_t I;
  uint16_t PC;
  c8_stack_t s = stack_init();
  init_chip(&regs[0], &I, &PC);

  /*   getting ROM file from args   */
  size_t filesize = 0;
  if(filepath) load_rom(filepath, &filesize, &mem[0]);
  else{
    fprintf(stderr, "ERROR: you forgot to provide a source file\n");
    exit(1);
  }

  /*   START OF WINDOWING   */
  InitWindow(SCREEN_WIDTH*10, SCREEN_HEIGHT*10, "chippy");
  SetTargetFPS(120);



  bool done_exec = false; //temporary; for debugging only

  while (!WindowShouldClose())
  {
    BeginDrawing();
    ClearBackground(BLACK);

    /*   EXECUTION   */
    if(!done_exec){
      union opcode op = { .raw = fetch(&mem[0], &PC) };
      printf("mem[PC]: %#x\nmem[PC+1]: %#x\nopcode: %#x\nPC: %d\n",
	     mem[PC-2], mem[PC-1], op.raw, PC);
      if(PC > (filesize + 0x200)) done_exec = true;

      switch(op.base_f.start){
      case 0x0:
	if(op.raw == 0x00E0){
	  ClearBackground(BLACK);
	}
	else if(op.raw == 0x00EE){
	  PC = stack_pop(&s); //return address
	}
	break;
      case 0x1:
	PC = op.ad_f.address;
	break;
      case 0x2:
	stack_push(&s, PC);
	PC = op.ad_f.address;
	break;
      case 0x3:
	if(regs[op.base_f.x] == op.const_f.const8) PC += 2;
	break;
      case 0x4:
	if(regs[op.base_f.x] != op.const_f.const8) PC += 2;
	break;
      case 0x5:
	if(regs[op.base_f.x] == regs[op.base_f.y]) PC += 2;
	break;
      case 0x6:
	regs[op.base_f.x] = op.const_f.const8;
	break;
      case 0x7:
	regs[op.base_f.x] += op.const_f.const8;
	break;
      case 0x8:
	switch(op.base_f.end){
	case 0x0:  //0x8XY0
	  regs[op.base_f.x] = regs[op.base_f.y];
	  break;
	case 0x1:  //0x8XY1
	  regs[op.base_f.x] |= regs[op.base_f.y];
	  break;
	case 0x2:  //0x8XY2
	  regs[op.base_f.x] &= regs[op.base_f.y];
	  break;
	case 0x3:  //0x8XY3
	  regs[op.base_f.x] ^= regs[op.base_f.y];
	  break;
	case 0x4:  //0x8XY4
	  regs[op.base_f.x] += regs[op.base_f.y];
	  break;
	case 0x5:  //0x8XY5
	  regs[op.base_f.x] -= regs[op.base_f.y];
	  break;
	case 0x6:  //0x8XY6
	  regs[op.base_f.x] = (regs[op.base_f.x] >> 1);
	  break;
	case 0x7:  //0x8XY7
	  regs[op.base_f.x] = (regs[op.base_f.y] - regs[op.base_f.x]);
	  break;
	case 0xE:  //0x8XYE
	  regs[op.base_f.x] = (regs[op.base_f.x] << 1);
	  break;
	default:
	  fprintf(stderr, "ERROR: Opcode %#X is not a valid operation.\n", op.raw);
	  break;
	}
	break;
      case 0x9:
	if(regs[op.base_f.x] != regs[op.base_f.y]) PC += 2;
	break;
      case 0xA:
	I = op.ad_f.address;
	break;
      case 0xB:
	PC = regs[0] + op.ad_f.address;
	break;
      case 0xC: {
	srand((unsigned) time(NULL));
	uint8_t randint = (uint8_t)rand();
	regs[op.base_f.x] = randint & op.const_f.const8;
	fprintf(stderr, "Random number:\t%#X\n", randint);
	break;
      }
      }}

    // stack debugging
    if(IsKeyPressed(KEY_P)){
      stack_pop(&s);
    }
    if(IsKeyPressed(KEY_O)){
      stack_push(&s, 0xFFFF);
    }
    if(IsKeyPressed(KEY_R)){
      init_chip(&regs[0], &I, &PC);
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

// initialize registers and clocks
void init_chip(uint8_t *regs, uint16_t *I, uint16_t *PC)
{
  for(int i = 0; i < 16; i++){
    regs[i] = 0x0000;
  }
  *I = 0x0000;
  *PC = 0x0200;
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
