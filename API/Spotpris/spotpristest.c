#include <stdio.h>
#include "Spotpris.h"
#include "../../libs/Pipes.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/stat.h>
//#include <sys/types.h>


// Enkelkommando för att kompilera just denna filen Todo - Can be removed?
// gcc -Wall -Wextra -std=c11 spotpristest.c spotpris.c ../Fetcher.c -lcurl -ljansson -D_POSIX_C_SOURCE=200112L -o spotpris_app

// Named pipe
#define FIFO_SPOTPRIS_WRITE "/tmp/fifo_spotpris"

int main(void)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);

    AllaSpotpriser spotpriser;
    
    int rc = Spotpris_FetchAll(&spotpriser);
    if (rc != 0)
    {
        fprintf(stderr, "Failed to fetch: %d\n", rc);
        return -4;
    }

    // Todo - Endast för debugging/testing, ta bort i prod
    AllaSpotpriser_Print(&spotpriser);

    
    
    mkfifo(FIFO_SPOTPRIS_WRITE, 0666);
    
    int spotpris_fd_write = open(FIFO_SPOTPRIS_WRITE, O_WRONLY);

    if (spotpris_fd_write < 0)
    {
        printf("Failed to open file: %s\n", FIFO_SPOTPRIS_WRITE);
        return -3;
    }    
    ssize_t bytesWritten = Pipes_WriteBinary(spotpris_fd_write, &spotpriser, sizeof(spotpriser));
    
    //printf("bytes skickade: %zd\n", bytesWritten);
        
    curl_global_cleanup();
    close(spotpris_fd_write);

    return 0;
}