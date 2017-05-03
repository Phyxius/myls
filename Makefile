CC = gcc
CFLAGS = -std=gnu11 -Wall -Werror -g -D_GNU_SOURCE

LIBFDROBJS = dllist.o fields.o jval.o jrb.o

myls:
	$(CC) $(CFLAGS) -o myls libmyls.c libmyls.h myls.c
