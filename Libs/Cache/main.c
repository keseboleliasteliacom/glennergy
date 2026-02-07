#include "InputCache.h"
#include "../Pipes.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#define FIFO_METEO_READ "/tmp/fifo_meteo_read"
#define FIFO_SPOTPRIS_READ "/tmp/fifo_spotpris"
#define FIFO_ALGORITHM_WRITE "/tmp/fifo_algoritm_write"

int main()
{
    InputCache inputCache;
    bool WorkDone = false;

    mkfifo(FIFO_ALGORITHM_WRITE, 0666);
    /*
        int meteo_fd_read = open(FIFO_METEO_READ, O_RDONLY);

        if (meteo_fd_read < 0)
        {
            printf("Failed to open file: %s\n", FIFO_METEO_READ);
            return -1;
        }
    */

    int spotpris_fd_read = open(FIFO_SPOTPRIS_READ, O_RDONLY);

    if (spotpris_fd_read < 0)
    {
        printf("Failed to open file: %s\n", FIFO_SPOTPRIS_READ);
        return -2;
    }

   /* int algorithm_fd_write = open(FIFO_ALGORITHM_WRITE, O_WRONLY);

    if (algorithm_fd_write < 0)
    {
        printf("Failed to open file: %s\n", FIFO_ALGORITHM_WRITE);
        return -3;
    }*/

    DagligSpotpris spotpris_test;

    while (1)
    {
        ssize_t bytesRead = Pipes_ReadBinary(spotpris_fd_read, &spotpris_test, sizeof(DagligSpotpris));

        if(bytesRead > 0)
        {
            printf("Got new data %d\n", spotpris_test.counter);
        }

        //ssize_t bytesWritten = Pipes_WriteBinary(algorithm_fd_write, &spotpris_test, sizeof(DagligSpotpris));
        
    }

    /* Disabled while testing READ functionality
    // Mocka ta emot meteo
    MeteoData meteo;
    meteo.temp = 5.5;
    inputCache.meteoData = meteo;

    // Mocka en tom spotpris data
    DagligSpotpris spotpris;
    inputCache.spotprisData = spotpris;

    // Läsa in user data/fastighetsdata från json.fil
    // Skip for now
    */

    // Write to named pipe
    // Disabled while testing READ functionality

    //close(algorithm_fd_write);

    // close(meteo_fd_read);
    close(spotpris_fd_read);

    // För att testa detta, köra "cat /tmp/fifo_algoritm_write" som en ny process

    return 0;
}