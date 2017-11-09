.PHONY: all clean

OBJS=main.o data.o file.o interface.o sys.o
CC:=gcc
override CFLAGS += -O3 -Wall -Wno-format -s

all: cvault

cvault: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) -c -o $@ $^

clean:
	rm -f $(OBJS)
