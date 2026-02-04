#include "InputCache.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

/*
Genom att använda absolut tid kan vi få följande flöde varje kvart, exempel nedan:
12:00:00
 - Meteo körs igång och gör sig grej, skickar struct hit via named pipe
 - Spotpris körs igång och gör sin grej, skickar struct hit via named pipe
 - På något sätt trackar vi om vi gjort vårt jobb den här kvarten, och den här countern eller statusen resettar vi varje kvart, samt memsetta structen
   - Behöver också tänka på att endast memsetta/resettea structen för meteo varje kvart, medans spotpris varje dygn

 Flödet för den här modulen blir att köra en loop och varje kvart:
 If - Har vi gjort jobb dne här kvarten?
    Nej - Har vi fått allt vi behöver från meteo, spotpris och userdata/fastighetdata.json?
      Nej - Invänta
      Ja - Gör sin grej, skriva ut, sleepa tills nästa absoluta kvarte
    Ja - Sleepa tills nästa absoluta kvart
*/

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

    int algorithm_fd_write = open(FIFO_ALGORITHM_WRITE, O_WRONLY);

    if (algorithm_fd_write < 0)
    {
        printf("Failed to open file: %s\n", FIFO_ALGORITHM_WRITE);
        return -3;
    }


    DagligSpotpris spotpris_test;
    ssize_t total = 0;
    ssize_t bytesRead;

    printf("test\n");
    while(total < sizeof(DagligSpotpris))
    {
        printf("test2\n");
        bytesRead = read(spotpris_fd_read, &spotpris_test + total, sizeof(DagligSpotpris) - total);
        printf("Vi har totalt %zd bytes och bytesRead är: %zd\n", total, bytesRead);

        if(bytesRead < 0)
        {
            printf("Failed to read\n");
        }

        total += bytesRead;
    }

    printf("Area code: %zd\n", sizeof(spotpris_test));
    printf("Area code: %d\n", spotpris_test.count);
    printf("area: %s", spotpris_test.area);

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
    //Disabled while testing READ functionality
    ssize_t n = write(algorithm_fd_write, &spotpris_test, sizeof(spotpris_test));
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

    close(algorithm_fd_write);


    //close(meteo_fd_read);
    close(spotpris_fd_read);

    // För att testa detta, köra "cat /tmp/fifo_algoritm_write" som en ny process

    return 0;
}
