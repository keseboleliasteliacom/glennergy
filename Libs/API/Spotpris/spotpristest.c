#include <stdio.h>
#include "Spotpris.h"
#include "../../Pipes.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

// Enkelkommando för att kompilera just denna filen
// gcc -Wall -Wextra -std=c11 spotpristest.c spotpris.c ../Fetcher.c -lcurl -ljansson -D_POSIX_C_SOURCE=200112L -o spotpris_app

// lägga till named pipes grejer
#define FIFO_SPOTPRIS_WRITE "/tmp/fifo_spotpris"

int main(void)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // Dessa olika delar kan ju vara egna metoder, så slipper main ha allt.
    // Men som en första version där vi konceptuellt testar så låter vi det vara.
    // Enkelt att kopiera runt och refactorera senare.

    // Skriva till named pipes

    mkfifo(FIFO_SPOTPRIS_WRITE, 0666);

    int spotpris_fd_write = open(FIFO_SPOTPRIS_WRITE, O_WRONLY);

    if (spotpris_fd_write < 0)
    {
        printf("Failed to open file: %s\n", FIFO_SPOTPRIS_WRITE);
        return -3;
    }

    const char *areas[4] = {"SE1", "SE2", "SE3", "SE4"};
    DagligSpotpris data;

    while (1)
    {
        for (int i = 0; i < 4; i++)
        {
            int rc = Spotpris_Fetch(&data, areas[i]);
            if (rc != 0)
            {
                fprintf(stderr, "Failed to fetch %s: %d\n", areas[i], rc);
                continue;
            }

            rc = Spotpris_SaveToFile(&data);
            if (rc != 0)
            {
                fprintf(stderr, "Failed to save %s\n", areas[i]);
            }
        }

        data.counter++;

        ssize_t bytesWritten = Pipes_WriteBinary(spotpris_fd_write, &data, sizeof(data));


        sleep(50);
    }
    curl_global_cleanup();
    close(spotpris_fd_write);
    // free(data.entries);

    return 0;
}