/*
 * C8ke : chip-8 emulator
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

#include "raylib.h"
#include "chip8.h"
#include "font.h"

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
  if(filepath)load_rom(filepath, &filesize);
  else{
    printf("you forgot to provide a source file\n");
    exit(1);
  }

  /*   START OF WINDOWING   */

  InitWindow(SCREEN_WIDTH*10, SCREEN_HEIGHT*10, "c8ke");
  SetTargetFPS(60);

  init_chip();
  bool done_exec = false; //temporary; for debugging only

  while (!WindowShouldClose())
  {
    BeginDrawing();
    ClearBackground(BLACK);

    /*   EXECUTION   */
    if(!done_exec){
      uint16_t opcode = fetch();
      printf("flash[PC]: %#x\nflash[PC+1]: %#x\nopcode: %#x\n",
	     flash[PC-2], flash[PC-1], opcode);
      if(PC > filesize) done_exec = true;

      if((opcode >> 12) == 0x6){
	uint8_t reg = ((opcode >> 8) & ~(0x60));
	uint8_t val = opcode & 0x00FF;
	regs[reg] = val;
      }
    }

    DrawText("Chip8 emulator", 10, 10, 40, RAYWHITE);
    DrawText(TextFormat("Registers: %#X %#X %#X %#X",
			regs[0], regs[1], regs[2], regs[3]), 10, 60, 20, RAYWHITE);
    

    EndDrawing();
  }

  CloseWindow();        // Close window and OpenGL context

  /*   END OF WINDOWING   */
  
  return 0;
}

// initialize registers and stack
void init_chip()
{
  for(int i = 0; i < 16; i++){
    regs[i] = 0x0000;
  }
  I = 0x0000;
  PC = 0x0000;
  stack_size = 1;
  stack = malloc(stack_size * sizeof(uint16_t));
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
  *filesize = fsize;

  if (fsize > FLASH_MAX_SIZE){
    printf("ERROR: File \"%s\" too big to fit in memory (%d). Either recompile c8ke with a bigger FLASH_MAX_SIZE or provide a smaller ROM.\n",
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

uint16_t fetch()
{
  uint16_t opcode = ((uint16_t)flash[PC] << 8) | ((uint16_t)flash[PC+1]);
  PC += 2;

  return opcode;
}


