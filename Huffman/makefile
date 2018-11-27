CC := gcc
CFLAGS := -O3 -std=c11 -Wall -Wno-unused-result

all: huffman

debug: huffman_debug

huffman: huffman.c
	${CC} ${CFLAGS} -o $@ $^

huffman_debug: huffman.c
	${CC} ${CFLAGS} -g3 -o $@ $^
