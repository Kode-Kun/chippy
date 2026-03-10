CC=cc
SRC=chip8.c
EXEC=chippy
INCLUDE=./include
CFLAGS=-Wall -Wextra

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
	RAYLIB_PATH=/home/kode/src/chippy/lib
	LDFLAGS=-L$(RAYLIB_PATH) -Wl,-rpath=$(RAYLIB_PATH) -lraylib
endif

ifeq ($(UNAME_S),Darwin)
	RAYLIB_PATH=/opt/homebrew/Cellar/raylib/5.5/lib
	FRAMEWORKS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	LDFLAGS=-L$(RAYLIB_PATH) -Wl, -lraylib $(FRAMEWORKS)
endif


all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(SRC) -o $(EXEC) $(CFLAGS) -I$(INCLUDE) $(LDFLAGS)

debug: $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) -I$(INCLUDE) $(LDFLAGS) -g

clean:
	rm -f $(EXEC)


.PHONY: all debug clean
