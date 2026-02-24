#define _XOPEN_SOURCE 700 
#define MODULE_NAME "INPUTCACHE"
#include "../Server/Log/Logger.h"
#include "Shm.h"
#include "InputCache.h"
#include "../Libs/Pipes.h"
#include <signal.h>
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
    log_Init("cache.log");
    bool WorkDone = false;

    InputCache_t *cache = malloc(sizeof(InputCache_t));
    if (!cache) {
        LOG_ERROR("malloc() Failed to allocate memory for InputCache");
        return -1;
    }
    memset(cache, 0, sizeof(InputCache_t));

    if (shm_Create() < 0) {
        LOG_ERROR("Failed to create shared memory");
        free(cache);
        return -1;
    }

    SharedCache_t *shm = shm_Attach();
    if (!shm) {
        LOG_ERROR("Failed to attach to shared memory");
        shm_Destroy();
        free(cache);
        return -1;
    }

    LOG_INFO("Loading homesystem file...");
    int loaded = homesystem_LoadAllCount(shm->input.home, "/etc/Glennergy-Fastigheter.json", MAX);
    shm->input.home_count = loaded;

    if (loaded < 0)
    {
        LOG_ERROR("Failed to load homesystem file");
        shm_Detach(shm);
        shm_Destroy();
        free(cache);
        return -1;
    }
    LOG_INFO("Loaded %d homesystem entries", loaded);

    // if (mkfifo(FIFO_ALGORITHM_WRITE, 0666) < 0 && errno != EEXIST)
    // {
    //     LOG_ERROR("Failed to create FIFO: %s", FIFO_ALGORITHM_WRITE);
    //     //free(cache);
    //     return -1;
    // }
    
    LOG_INFO("InputCache started, waiting for data...");

    while(1)
    {
        if (WorkDone) {
            LOG_INFO("Work done, sleeping for 10 seconds...");
            sleep(10);
            WorkDone = false;
            continue;
        }

        MeteoData meteo_test;
        AllaSpotpriser spotpris_test;
        // --- METEO ---
        LOG_INFO("Waiting for meteo data...");

        int meteo_fd_read = open(FIFO_METEO_READ, O_RDONLY);
        if (meteo_fd_read < 0)
        {
            LOG_ERROR("Failed to open file: %s", FIFO_METEO_READ);
            sleep(5);
            continue;
        }
        
        ssize_t bytesReadMeteo = Pipes_ReadBinary(meteo_fd_read, &meteo_test, sizeof(MeteoData));
        close(meteo_fd_read);

        if (bytesReadMeteo == sizeof(MeteoData))
        {
            LOG_INFO("Got new data meteo %zd", bytesReadMeteo);
            LOG_INFO("Meteo data count: %zu", meteo_test.pCount);

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
                
            }
            LOG_INFO("meteo after copy: %zu meteodata entries", cache->meteo_count);
            InputCache_SaveMeteo(&meteo_test);
        } else {
            LOG_ERROR("failed to read meteo data, got %zd bytes", bytesReadMeteo);
        }

        // --- SPOTPRIS ---
        LOG_INFO("Waiting for spotpris data...");
        int spotpris_fd_read = open(FIFO_SPOTPRIS_READ, O_RDONLY);
        if (spotpris_fd_read < 0)
        {
            LOG_ERROR("Failed to open file: %s", FIFO_SPOTPRIS_READ);
            sleep(5);
            continue;
        }

        
        ssize_t bytesReadSpotpris = Pipes_ReadBinary(spotpris_fd_read, &spotpris_test, sizeof(AllaSpotpriser));
        close(spotpris_fd_read);
        
        if (bytesReadSpotpris == sizeof(AllaSpotpriser))
        {
            LOG_INFO("Got new data spotpris %zd", bytesReadSpotpris);
        } else {
            LOG_ERROR("failed to read spotpris data, got %zd bytes", bytesReadSpotpris);
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
            
                LOG_INFO("Area %s: %zu price entries copied", area_names[area], cache->spotpris.count[area]);
            }

        InputCache_SaveSpotpris(&spotpris_test);
            
        // LOG_INFO("Sending complete packet to algorithm...");
        // if (InputCache_PipeToAlgorithm(cache) != 0)
        // {
        //     LOG_ERROR("Failed to pipe data to algorithm\n");
        // }

        LOG_INFO("Writing data to shared memory...");

        if (shm_Lock_Input_Write(shm) < 0) {
            LOG_ERROR("Failed to lock shared memory for writing");
        } else {
            shm->input.meteo_count = cache->meteo_count;
            memcpy(shm->input.meteo, cache->meteo, sizeof(cache->meteo));
            shm->input.spotpris = cache->spotpris;
            
            shm->last_input_update = time(NULL);
            shm_Unlock_Input_Write(shm);
            LOG_INFO("Data written to shared memory");
        }

        
        printf("cleaning up...\n");
        LOG_INFO("Cycle complete, ready for next data...");
        // AllaSpotpriser_Print(&spotpris_test);
        WorkDone = true;
    }

    // close(algorithm_fd_write);
    shm_Detach(shm);
    shm_Destroy();
    free(cache);
    //unlink(FIFO_ALGORITHM_WRITE);
    log_Cleanup();
    return 0;
}