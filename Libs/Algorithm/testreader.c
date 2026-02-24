#define MODULE_NAME "TESTREADER"
#include "../../Server/Log/Logger.h"
#include "testreader.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "average.h"
#include <errno.h>

#include "../../Cache/InputCache.h"
#include "../Pipes.h"

#define FIFO_ALGORITHM_READ "/tmp/fifo_algoritm"

//gcc -Wall -Wextra -std=c11 -g testreader.c ../Pipes.c -I../../ -o testreader

int test_reader() {

    InputCache_t *cache = malloc(sizeof(InputCache_t));
    if (!cache) {
        LOG_ERROR("malloc() Failed to allocate memory for InputCache");
        return -1;
    }

    while(1)
    {
    memset(cache, 0, sizeof(InputCache_t));

    int fifo_fd = open(FIFO_ALGORITHM_READ, O_RDONLY);
    if (fifo_fd < 0) {
        LOG_ERROR("Failed to open FIFO %s: %s", FIFO_ALGORITHM_READ, strerror(errno));
        continue;  // Skip to next iteration instead of returning
    }

    ssize_t bytes_read = Pipes_ReadBinary(fifo_fd, cache, sizeof(InputCache_t));

    close(fifo_fd);
    //unlink(FIFO_ALGORITHM_READ);

    if (bytes_read != sizeof(InputCache_t)) {
        LOG_ERROR("Failed to read complete data (got %zd, expected %zu bytes)", bytes_read, sizeof(InputCache_t));
        continue;
    }
    LOG_INFO("Received from cache Meteo: %zu HomeSystem: %zu price areas: %zu", cache->meteo_count, cache->home_count, sizeof(cache->spotpris.count) / sizeof(cache->spotpris.count[0]));

    for (size_t i = 0; i < cache->meteo_count; i++) {

        for (size_t j = 0; j < 2 && j < KVARTAR_TOTALT; j++) {
            printf("ID: %d, city: %s, lat: %.2f, lon: %.2f, time: %s, temp: %.2f, GHI: %.2f, cloud: %.2f\n", 
                   cache->meteo[i].id,
                   cache->meteo[i].city, 
                   cache->meteo[i].lat, 
                   cache->meteo[i].lon,
                   cache->meteo[i].sample[j].time_start,
                   cache->meteo[i].sample[j].temp,
                   cache->meteo[i].sample[j].ghi,
                   cache->meteo[i].sample[j].cloud_cover);
        }

    }
    for (size_t i = 0; i < cache->home_count; i++) {
        printf("FROM HOME id: %d: %s (lat: %.2f, lon: %.2f) Panel: capacity: %.2f kWh, tilt: %.2f degrees, electricity_area: %s\n", 
               cache->home[i].id,
               cache->home[i].city, 
               cache->home[i].lat, 
               cache->home[i].lon,
               cache->home[i].panel_capacitykwh,
               cache->home[i].panel_tiltdegrees,
               cache->home[i].electricity_area);
    }

    const char *area_names[AREA_COUNT] = {"SE1", "SE2", "SE3", "SE4"};

    for (size_t area_idx = 0; area_idx < 4; area_idx++)
    {
        printf("\n=== Area %s ===\n", area_names[area_idx]);
        printf("Total kvartar: %zu\n", cache->spotpris.count[area_idx]);
    
        size_t show_count = cache->spotpris.count[area_idx];
        if (show_count > 10) show_count = 10;  // Show only first 10
        
        for (size_t entry = 0; entry < show_count; entry++) {
            printf("  [%2zu] %s -> %.5f SEK/kWh\n", 
                   entry,
                   cache->spotpris.data[area_idx][entry].time_start,
                   cache->spotpris.data[area_idx][entry].sek_per_kwh);
        }
    
        if (cache->spotpris.count[area_idx] > 10) {
            printf("  ... (%zu more entries)\n", 
           cache->spotpris.count[area_idx] - 10);
        }
    }

    SpotStats_t spotpris_stats;
    // AlgoInfluencer_t influencer = {
    //     .spotpris = &spotpris_stats
    // };
    int result = average_SpotprisStats(&spotpris_stats, cache);

    int window_result = average_WindowLow(cache, spotpris_stats.area[0].q25);

    LOG_INFO("Sleeping for 10 seconds before next read...");
    sleep(10);
}   //while
    printf("Free cache\n");
    free(cache);
    return 0;
}