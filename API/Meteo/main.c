#include "Meteo.h"
#include "../../Libs/Pipes.h"
#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>
#include <fcntl.h>
#include <unistd.h>
#include "../../Libs/Utils/utils.h"

#define FIFO_METEO_WRITE "/tmp/fifo_meteo"

int main()
{
    mkfifo(FIFO_METEO_WRITE, 0666);

    setvbuf(stdout, NULL, _IONBF, 0);
    int meteo_fd_write = open(FIFO_METEO_WRITE, O_WRONLY);

    if (meteo_fd_write < 0)
    {
        printf("Failed to open file: %s\n", FIFO_METEO_WRITE);
        return -3;
    }

    printf("nu börjar loopen\n");

    static time_t last_modified = -1;

    MeteoData data;
    Meteo_LoadGlennergy(&data);

    if (file_lastModified("/etc/Glennergy-Fastigheter.json", &last_modified) == 1)
    {
        Meteo_LoadGlennergy(&data);
        printf("Info changed, reloaded file.\n");
    }

    meteo_Fetch(&data);

    ssize_t bytesWritten = Pipes_WriteBinary(meteo_fd_write, &data, sizeof(data));

    printf("bytes skickade: %zd\n", bytesWritten);

    close(meteo_fd_write);
    unlink(FIFO_METEO_WRITE);

    return 0;
}