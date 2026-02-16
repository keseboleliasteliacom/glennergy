#define MODULE_NAME "INPUTCACHE"
#include "../Server/Log/Logger.h"
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
    log_Init(NULL);
    InputCache inputCache;
    bool WorkDone = false;

    mkfifo(FIFO_ALGORITHM_WRITE, 0666);

    MeteoData meteo_test;
    AllaSpotpriser spotpris_test;


    LOG_INFO("InputCache test");
    while (1)
    {
        // --- METEO ---
        int meteo_fd_read = open(FIFO_METEO_READ, O_RDONLY);

        if (meteo_fd_read < 0)
        {
            continue;
        }

        ssize_t bytesReadMeteo = Pipes_ReadBinary(meteo_fd_read, &meteo_test, sizeof(MeteoData));

        if (bytesReadMeteo > 0)
        {
            for (int i = 0; i < 4; i++)
            {
                printf("Got new data Meteo %f\n", meteo_test.pInfo[i].lat);
            }
            InputCache_SaveMeteo(&meteo_test);
        }

        // --- SPOTPRIS ---

        int spotpris_fd_read = open(FIFO_SPOTPRIS_READ, O_RDONLY);

        if (spotpris_fd_read < 0)
        {
            continue;
        }

        ssize_t bytesReadSpotpris = Pipes_ReadBinary(spotpris_fd_read, &spotpris_test, sizeof(AllaSpotpriser));

        if (bytesReadSpotpris > 0)
        {
            printf("Got new data spotpris %zd\n", bytesReadSpotpris);
            InputCache_SaveSpotpris(&spotpris_test);
        }

        // --- ALGORITM ---
        /* int algorithm_fd_write = open(FIFO_ALGORITHM_WRITE, O_WRONLY);

         if (algorithm_fd_write < 0)
         {
             printf("Failed to open file: %s\n", FIFO_ALGORITHM_WRITE);
             return -3;
         }*/

        // AllaSpotpriser_Print(&spotpris_test);

        // close(algorithm_fd_write);
        close(meteo_fd_read);
        close(spotpris_fd_read);
    }

    return 0;
}