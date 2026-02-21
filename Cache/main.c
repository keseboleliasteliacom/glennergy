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
#include <sys/stat.h>
//#include <sys/types.h>
//#include "../Libs/Homesystem.h"
// #define FIFO_METEO_READ "/tmp/fifo_meteo"
// #define FIFO_SPOTPRIS_READ "/tmp/fifo_spotpris"
// #define FIFO_ALGORITHM_WRITE "/tmp/fifo_algoritm_write"

const char *area_names[AREA_COUNT] = {"SE1", "SE2", "SE3", "SE4"};

int main()
{
    log_Init(NULL);
    InputCache_t *cache = malloc(sizeof(InputCache_t));
    bool WorkDone = false;
    
    if (!cache) {
        fprintf(stderr, "Failed to allocate memory\n");
        return -1;
    }
    memset(cache, 0, sizeof(InputCache_t));

    printf("Loading homesystem file..\n");
    int loaded = homesystem_LoadAllCount(cache->home, "/etc/Glennergy-Fastigheter.json", MAX);
    if (loaded < 0)
    {
        fprintf(stderr, "Failed to load homesystem file\n");
        free(cache);
        return -1;
    }
    cache->home_count = loaded;

    if (mkfifo(FIFO_ALGORITHM_WRITE, 0666) < 0 && errno != EEXIST)
    {
        printf("Failed to create FIFO: %s\n", FIFO_ALGORITHM_WRITE);
        free(cache);
        return -1;
    }
    
    LOG_INFO("InputCache started, waiting for data...");
    MeteoData meteo_test;
    AllaSpotpriser spotpris_test;
    // --- METEO ---
    int meteo_fd_read = open(FIFO_METEO_READ, O_RDONLY);
    if (meteo_fd_read < 0)
    {
        printf("Failed to open file: %s\n", FIFO_METEO_READ);
        free(cache);
        return -1;
    }
    
    ssize_t bytesReadMeteo = Pipes_ReadBinary(meteo_fd_read, &meteo_test, sizeof(MeteoData));
    close(meteo_fd_read);

    if (bytesReadMeteo == sizeof(MeteoData))
    {
        printf("Got new data meteo %zd\n", bytesReadMeteo);
        printf("Meteo data count: %zu\n", meteo_test.pCount);

        cache->meteo_count = meteo_test.pCount;
        for (size_t i = 0; i < cache->meteo_count; i++)
        {
            cache->meteo[i].id = meteo_test.pInfo[i].id;
            strncpy(cache->meteo[i].city, meteo_test.pInfo[i].property_name, NAME_MAX);
            cache->meteo[i].lat = meteo_test.pInfo[i].lat;
            cache->meteo[i].lon = meteo_test.pInfo[i].lon;
        
            printf("Got new data Meteo ID:%d City:%s\n", cache->meteo[i].id, cache->meteo[i].city);
            // Copy the samples array
            memcpy(cache->meteo[i].sample, meteo_test.pInfo[i].sample, sizeof(Samples) * KVARTAR_TOTALT);
            
            for (int i = 0; i < cache->meteo_count; i++)
            {
            }
        }
        printf("New data meteo: %zu meteodata entries\n", cache->meteo_count);
         InputCache_SaveMeteo(&meteo_test);
    } else {
        fprintf(stderr, "failed to read meteo data, got %zd bytes\n", bytesReadMeteo);
    }

    // --- SPOTPRIS ---
    
    int spotpris_fd_read = open(FIFO_SPOTPRIS_READ, O_RDONLY);
    if (spotpris_fd_read < 0)
    {
        printf("Failed to open file: %s\n", FIFO_SPOTPRIS_READ);
        free(cache);
        return -1;
    }

    
    ssize_t bytesReadSpotpris = Pipes_ReadBinary(spotpris_fd_read, &spotpris_test, sizeof(AllaSpotpriser));
    close(spotpris_fd_read);
    
    if (bytesReadSpotpris == sizeof(AllaSpotpriser))
    {
        printf("Got new data spotpris %zd\n", bytesReadSpotpris);
    } else {
        fprintf(stderr, "failed to read spotpris data, got %zd bytes\n", bytesReadSpotpris);
    }

        for (int area = 0; area < AREA_COUNT; area++)
        {
            //strncpy(cache->spotpris.areas[i].areaname, spotpris_test.areas[i].areaname, 4);

            cache->spotpris.count[area] = spotpris_test.areas[area].count;
            
            for (size_t entry = 0; entry < spotpris_test.areas[area].count; entry++)
            {
            strncpy(cache->spotpris.data[area][entry].time_start, spotpris_test.areas[area].kvartar[entry].time_start, 32);
                    cache->spotpris.data[area][entry].sek_per_kwh = spotpris_test.areas[area].kvartar[entry].sek_per_kwh;
            }
        
            printf("  Area %s: %zu price entries copied\n", area_names[area], cache->spotpris.count[area]);
        }

    InputCache_SaveSpotpris(&spotpris_test);
        
    printf("Sending complete packet to algorithm...\n");
    if (InputCache_PipeToAlgorithm(cache) != 0)
    {
        fprintf(stderr, "Failed to pipe data to algorithm\n");
        free(cache);
        return -3;
    }

    
    printf("cleaning up...\n");

    // AllaSpotpriser_Print(&spotpris_test);
    
    
    // close(algorithm_fd_write);
    free(cache);
    close(meteo_fd_read);
    close(spotpris_fd_read);
    unlink(FIFO_ALGORITHM_WRITE);

    return 0;
}