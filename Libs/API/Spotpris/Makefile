# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -D_POSIX_C_SOURCE=200112L

# Källkoder
CACHE_DIR = .
API_DIR = ../API

# Körbar fil
TARGET = Spotpris

# Default target
all:
	$(CC) $(CFLAGS) Spotpris.c spotpristest.c ../../Pipes.c ../../Fetcher.c -o $(TARGET) -lcurl -ljansson

# Rensa körbar fil
clean:
	rm -f $(TARGET)