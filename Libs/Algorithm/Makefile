CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -pthread
LDFLAGS = -pthread -lrt

TARGET = testreader
SRC = testreader.c average.c
CACHE_DIR = ../../Cache
OBJS = testreader.o average.o Shm.o Logger.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

testreader.o: testreader.c testreader.h average.h
	$(CC) $(CFLAGS) -c testreader.c -I../..

average.o: average.c average.h
	$(CC) $(CFLAGS) -c average.c -I../..

Shm.o: $(CACHE_DIR)/Shm.c $(CACHE_DIR)/Shm.h
	$(CC) $(CFLAGS) -c $(CACHE_DIR)/Shm.c -I../..

Logger.o: ../../Server/Log/Logger.c ../../Server/Log/Logger.h
	$(CC) $(CFLAGS) -c ../../Server/Log/Logger.c -I../..

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean