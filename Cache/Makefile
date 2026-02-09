# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -D_POSIX_C_SOURCE=200112L

# Källkoder
CACHE_DIR = .
API_DIR = ../API

# Körbar fil
TARGET = InputCache

# Default target
all:
	$(CC) $(CFLAGS) $(CACHE_DIR)/InputCache.c $(CACHE_DIR)/main.c $(API_DIR)/Meteo/Meteo.c $(API_DIR)/Spotpris/Spotpris.c ../Libs/Fetcher.c ../Libs/Pipes.c -o $(TARGET) -lcurl -ljansson

# Rensa körbar fil
clean:
	rm -f $(TARGET)
