# SystemInfo Makefile

# Compilation Parameters
CC=gcc
CFLAGS= -Iincludes -Wextra -Wall
SOURCES= systeminfo.c

all:
	$(CC) $(SOURCES) $(CFLAGS) -o systeminfo
clean:
	rm -f systeminfo
