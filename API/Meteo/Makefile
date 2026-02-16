# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -D_POSIX_C_SOURCE=200112L

# Källkoder
CACHE_DIR = .
API_DIR = ../API

# Körbar fil
TARGET = Meteo

# Default target
all:
	$(CC) $(CFLAGS) main.c Meteo.c ../../Libs/Pipes.c ../../Server/Log/Logger.c ../../Libs/Fetcher.c -o $(TARGET) -lcurl -ljansson

# Rensa körbar fil
clean:
	rm -f $(TARGET)