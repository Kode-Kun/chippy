CC=cc
SRC=chip8.c
EXEC=chippy
RAYLIB_PATH=/home/kode/src/chippy/lib
INCLUDE=./include
FRAMEWORKS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

CFLAGS=-Wall -Wextra -I$(INCLUDE)
LDFLAGS=-L$(RAYLIB_PATH) -Wl,-rpath=$(RAYLIB_PATH) -lraylib

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(SRC) -o $(EXEC) $(CFLAGS) $(LDFLAGS)

debug: $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LDFLAGS) -g

clean:
	rm -f $(EXEC)


.PHONY: all debug clean
