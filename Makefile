CC=cc
SRC=chip8.c
TARGET=c8ke
CFLAGS=-Wall -Wextra

default:
	${CC} ${SRC} -o ${TARGET} ${CFLAGS} -g
