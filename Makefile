CC=cc
SRC=chip8.c
TARGET=c8ke
RAYLIB_PATH=/opt/homebrew/Cellar/raylib/5.5/lib
CFLAGS=-Wall -Wextra -L${RAYLIB_PATH} -lraylib

default:
	${CC} ${SRC} -o ${TARGET} ${CFLAGS}

debug:
	${CC} ${SRC} -o ${TARGET} ${CFLAGS} -g
