#define _XOPEN_SOURCE 700
#define MODULE_NAME "TESTREADER"
#include "../../Server/Log/Logger.h"
#include "testreader.h"
#include "../../Cache/Shm.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "average.h"

// #include "../../Cache/InputCache.h"
// #include "../Pipes.h"

// #define FIFO_ALGORITHM_READ "/tmp/fifo_algoritm"

//gcc -Wall -Wextra -std=c11 -g testreader.c ../Pipes.c -I../../ -o testreader

int test_reader() {

    log_Init("testreader.log");
    static InputCache_t cachecopy;
    // InputCache_t *cache = malloc(sizeof(InputCache_t));
    // if (!cache) {
    //     LOG_ERROR("malloc() Failed to allocate memory for InputCache");
    //     return -1;
    // }

    SharedCache_t *shm = shm_Attach();
    if (!shm) {
        LOG_ERROR("Failed to attach to shared memory");
        return -1;
    }
    LOG_INFO("Attached to shared memory, starting loop...");

    while(1)
    {
    // memset(cache, 0, sizeof(InputCache_t));
    // int fifo_fd = open(FIFO_ALGORITHM_READ, O_RDONLY);
    // ssize_t bytes_read = Pipes_ReadBinary(fifo_fd, cache, sizeof(InputCache_t));
    // close(fifo_fd);
    // //unlink(FIFO_ALGORITHM_READ);
    // if (bytes_read != sizeof(InputCache_t)) {
    //     LOG_ERROR("Failed to read complete data (got %zd, expected %zu bytes)", bytes_read, sizeof(InputCache_t));
    //     return -1;
    // }
    // LOG_INFO("Received from cache Meteo: %zu HomeSystem: %zu price areas: %zu", cache->meteo_count, cache->home_count, sizeof(cache->spotpris.count) / sizeof(cache->spotpris.count[0]));
    shm_Lock_Input_Read(shm); // 
    memcpy(&cachecopy, &shm->input, sizeof(InputCache_t));
    LOG_INFO("Received from shm Meteo: %zu HomeSystem: %zu", cachecopy.meteo_count, cachecopy.home_count);
    shm_Unlock_Input_Read(shm);

    for (size_t i = 0; i < cachecopy.meteo_count; i++) {

        for (size_t j = 0; j < 2 && j < KVARTAR_TOTALT; j++) {
            printf("ID: %d, city: %s, lat: %.2f, lon: %.2f, time: %s, temp: %.2f, GHI: %.2f, cloud: %.2f\n", 
                   cachecopy.meteo[i].id,
                   cachecopy.meteo[i].city, 
                   cachecopy.meteo[i].lat, 
                   cachecopy.meteo[i].lon,
                   cachecopy.meteo[i].sample[j].time_start,
                   cachecopy.meteo[i].sample[j].temp,
                   cachecopy.meteo[i].sample[j].ghi,
                   cachecopy.meteo[i].sample[j].cloud_cover);
        }

    }
    for (size_t i = 0; i < cachecopy.home_count; i++) {
        printf("FROM HOME id: %d: %s (lat: %.2f, lon: %.2f) Panel: capacity: %.2f kWh, tilt: %.2f degrees, electricity_area: %s\n", 
               cachecopy.home[i].id,
               cachecopy.home[i].city, 
               cachecopy.home[i].lat, 
               cachecopy.home[i].lon,
               cachecopy.home[i].panel_capacitykwh,
               cachecopy.home[i].panel_tiltdegrees,
               cachecopy.home[i].electricity_area);
    }

    const char *area_names[AREA_COUNT] = {"SE1", "SE2", "SE3", "SE4"};

    for (size_t area_idx = 0; area_idx < 4; area_idx++)
    {
        printf("\n=== Area %s ===\n", area_names[area_idx]);
        printf("Total kvartar: %zu\n", cachecopy.spotpris.count[area_idx]);
    
        size_t show_count = cachecopy.spotpris.count[area_idx];
        if (show_count > 10) show_count = 10;  // Show only first 10
        
        for (size_t entry = 0; entry < show_count; entry++) {
            printf("  [%2zu] %s -> %.5f SEK/kWh\n", 
                   entry,
                   cachecopy.spotpris.data[area_idx][entry].time_start,
                   cachecopy.spotpris.data[area_idx][entry].sek_per_kwh);
        }
    
        if (cachecopy.spotpris.count[area_idx] > 10) {
            printf("  ... (%zu more entries)\n", 
           cachecopy.spotpris.count[area_idx] - 10);
        }
    }

    SpotStats_t spotpris_stats;
    int result = average_SpotprisStats(&spotpris_stats, &cachecopy);
    int window_result = average_WindowLow(&cachecopy, spotpris_stats.area[0].q25);

    LOG_INFO("Sleeping for 10 seconds before next read...");
    sleep(10);
}   //while
    printf("Free cache\n");
    //free(cache);
    shm_Detach(shm);
    return 0;
}


int main() {
    log_Init("testreader.log");
    LOG_INFO("Starting testreader...");
    
    int result = test_reader();
    
    log_Cleanup();
    return result;
}