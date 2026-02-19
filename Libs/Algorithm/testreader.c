#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "../../Cache/InputCache.h"
#include "../Pipes.h"

#define FIFO_ALGORITHM_READ "/tmp/fifo_algoritm_write"

//gcc -Wall -Wextra -std=c11 -g testreader.c ../Pipes.c -o testreader


int main () {


    InputCache *cache = malloc(sizeof(InputCache));
    if (!cache) {
        fprintf(stderr, "Failed to allocate memory for InputCache\n");
        return -1;
    }

    memset(cache, 0, sizeof(InputCache));

    int fifo_fd = open(FIFO_ALGORITHM_READ, O_RDONLY);

    ssize_t bytes_read = Pipes_ReadBinary(fifo_fd, cache, sizeof(InputCache));

    close(fifo_fd);
    unlink(FIFO_ALGORITHM_READ);

    if (bytes_read != sizeof(InputCache)) {
        fprintf(stderr, "Failed to read complete data (got %zd, expected %zu bytes)\n",
                bytes_read, sizeof(InputCache));
        return -1;
    }
    printf("Received from cache: %zu properties, 4 price areas\n", cache->meteoData.pCount);

}