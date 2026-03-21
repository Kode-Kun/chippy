BUILD_DIR = build

LIB_SRCS = common.c
LIB_OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(LIB_SRCS))

# Emulator
EXECC = chippy
EXECC_SRC = chip8.c
EXECC_OBJ = $(BUILD_DIR)/chip8.o

# Assembler
EXECA = chasm
EXECA_SRC = asm.c
EXECA_OBJ = $(BUILD_DIR)/asm.o

INCLUDE=./include
CFLAGS=-Wall -Wextra
WRITE=write

UNAME_S := $(shell uname -s)
WD := $(shell pwd)

ifeq ($(UNAME_S),Linux)
	CC=gcc
	RAYLIB_PATH=$(WD)/lib
	LDFLAGS=-L$(RAYLIB_PATH) -Wl,-rpath=$(RAYLIB_PATH) -lraylib
endif

ifeq ($(UNAME_S),Darwin)
	CC=clang
	RAYLIB_PATH=/opt/homebrew/Cellar/raylib/5.5/lib
	FRAMEWORKS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	LDFLAGS=-L$(RAYLIB_PATH) -Wl, -lraylib $(FRAMEWORKS)
endif

$(EXECC): $(EXECC_OBJ) $(LIB_OBJS)
	$(CC) $(CFLAGS) -I$(INCLUDE) $^ -o $@ -g $(LDFLAGS)

$(EXECA): $(EXECA_OBJ) $(LIB_OBJS)
	$(CC) $(CFLAGS) -I$(INCLUDE) $^ -o $@ -g

$(WRITE): $(WRITE).c
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -f $(EXECC) $(WRITE)
	rm -rf $(BUILD_DIR)


.PHONY: all debug clean
