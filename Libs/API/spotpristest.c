#include <stdio.h>
#include "Spotpris.h"
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

    

    const char *areas[4] = {"SE1", "SE2", "SE3", "SE4"};
    DagligSpotpris data;
    for (int i = 0; i < 1; i++) {
        int rc = Spotpris_Fetch(&data, areas[i]);
        if (rc != 0) {
            fprintf(stderr, "Failed to fetch %s: %d\n", areas[i], rc);
            continue;
        }

        rc = Spotpris_SaveToFile(&data);
        if (rc != 0) {
            fprintf(stderr, "Failed to save %s\n", areas[i]);
        }

        //free(data.entries);
    }

    curl_global_cleanup();



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

    ssize_t n = write(spotpris_fd_write, &data, sizeof(data));
    if (n < 0)
    {

        if (errno == EAGAIN)
        {
            printf("Reader is full\n");
        }
        perror("write");
    }
    else
    {
        printf("Wrote %zd bytes to the pipe\n", n);
    }

    close(spotpris_fd_write);
    
    return 0;
}