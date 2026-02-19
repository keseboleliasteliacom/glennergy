#include "InputCache.h"
#include "../Libs/Pipes.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#define FIFO_METEO_READ "/tmp/fifo_meteo"
#define FIFO_SPOTPRIS_READ "/tmp/fifo_spotpris"
#define FIFO_ALGORITHM_WRITE "/tmp/fifo_algoritm_write"

int main()
{
    InputCache *input_cache = malloc(sizeof(InputCache));
    bool WorkDone = false;
    
    if (!input_cache) {
        fprintf(stderr, "Failed to allocate memory\n");
        free(input_cache);
        return -1;
    }
    memset(input_cache, 0, sizeof(InputCache));

    unlink(FIFO_ALGORITHM_WRITE);
    if (mkfifo(FIFO_ALGORITHM_WRITE, 0666) < 0 && errno != EEXIST)
    {
        printf("Failed to create FIFO: %s\n", FIFO_ALGORITHM_WRITE);
        free(input_cache);
        return -1;
    }
    
    // MeteoData meteo_test;
    // AllaSpotpriser spotpris_test;


    // --- METEO ---
    int meteo_fd_read = open(FIFO_METEO_READ, O_RDONLY);

    if (meteo_fd_read < 0)
    {
        printf("Failed to open file: %s\n", FIFO_METEO_READ);
        free(input_cache);
        return -1;
    }

    ssize_t bytesReadMeteo = Pipes_ReadBinary(meteo_fd_read, &input_cache->meteoData, sizeof(MeteoData));

    if (bytesReadMeteo > 0)
    {
        for (int i = 0; i < 4; i++)
        {
            printf("Got new data Meteo %zd\n", input_cache->meteoData.pInfo[i].id);
        }
        InputCache_SaveMeteo(&input_cache->meteoData);
    }

    // --- SPOTPRIS ---
    
    int spotpris_fd_read = open(FIFO_SPOTPRIS_READ, O_RDONLY);
    
    if (spotpris_fd_read < 0)
    {
        printf("Failed to open file: %s\n", FIFO_SPOTPRIS_READ);
        return -2;
    }

    
    ssize_t bytesReadSpotpris = Pipes_ReadBinary(spotpris_fd_read, &input_cache->spotprisData, sizeof(AllaSpotpriser));

    if (bytesReadSpotpris > 0)
    {
        printf("Got new data spotpris%zd\n", bytesReadSpotpris);
    }
    
    InputCache_SaveSpotpris(&input_cache->spotprisData);
    
    
    
    int algorithm_fd_write = open(FIFO_ALGORITHM_WRITE, O_WRONLY);
     if (algorithm_fd_write < 0)
     {
         printf("Failed to open FIFO: %s\n", FIFO_ALGORITHM_WRITE);
         return -3;
     }

    if (InputCache_PipeToAlgorithm(algorithm_fd_write, input_cache) == 0) {
        printf("Data sent to algorithm\n");
    } else {
        fprintf(stderr, "Failed to send data to algorithm\n");
    }

    
    printf("cleaning up...\n");
    
    // AllaSpotpriser_Print(&spotpris_test);
    
    
    // close(algorithm_fd_write);
    free(input_cache);
    close(meteo_fd_read);
    close(spotpris_fd_read);
    close(algorithm_fd_write);

    return 0;
}