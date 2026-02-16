#include "Meteo.h"
#include "../../Libs/Pipes.h"
#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>
#include <fcntl.h>
#include <unistd.h>
#include "../../Libs/Utils/utils.h"
#include "../../Server/Log/Logger.h"

#define FIFO_METEO_WRITE "/tmp/fifo_meteo"
static const char* MODULE_NAME = "METEO main.c";
int main()
{

    if (log_Init(NULL) < 0)
    {
        fprintf(stderr, "Failed to initialize logger\n");
        return -1;
    }

    log_SetLevel(LOG_LEVEL_INFO);
    LOG_INFO("Meteo test started");


    mkfifo(FIFO_METEO_WRITE, 0666);

    int meteo_fd_write = open(FIFO_METEO_WRITE, O_WRONLY);

    if (meteo_fd_write < 0)
    {
        LOG_ERROR("Failed to open FIFO for writing");
        log_Cleanup();
        printf("Failed to open file: %s\n", FIFO_METEO_WRITE);
        return -3;
    }

    LOG_INFO("FIFO opened for writing");

    printf("nu börjar loopen\n");

    static time_t last_modified = -1;

    MeteoData data;
    Meteo_LoadPropertyInfo(&data);

    LOG_DEBUG("Initial property info loaded");

    /*
    while (1)
    {
    */

    if(file_lastModified("fastighets_test.json", &last_modified) == 1)
    {
        Meteo_LoadPropertyInfo(&data);
        LOG_INFO("Property info changed and reloaded");
        printf("Info changed, reloaded file.\n");
    }

    LOG_INFO("Fetching meteo data");
    meteo_Fetch(&data);


    ssize_t bytesWritten = Pipes_WriteBinary(meteo_fd_write, &data, sizeof(data));

    printf("bytes skickade: %zd\n", bytesWritten);
    sleep(10);
    //}
    close(meteo_fd_write);
    unlink(FIFO_METEO_WRITE);

    LOG_INFO("Meteo test shutting down");
    log_CloseWrite();
    log_Cleanup();

    return 0;
}