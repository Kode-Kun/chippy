CC=cc
SRC=chip8.c
EXEC=chippy
RAYLIB_PATH=/opt/homebrew/Cellar/raylib/5.5/lib
CFLAGS=-Wall -Wextra -L$(RAYLIB_PATH) -lraylib
FRAMEWORKS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(SRC) -o $(EXEC) $(CFLAGS) $(FRAMERWORKS)

debug: $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) -g $(FRAMERWORKS)

clean:
	rm -f $(EXEC)


.PHONY: all debug clean
