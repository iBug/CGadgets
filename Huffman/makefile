CC := gcc
CFLAGS ?= -O3 -Wall

all: huffman

debug: huffman_debug

huffman: huffman.c
	${CC} ${CFLAGS} -o $@ $^

huffman_debug: huffman.c
	${CC} ${CFLAGS} -g3 -o $@ $^
