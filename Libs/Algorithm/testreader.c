#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "../../Cache/InputCache.h"
#include "../Pipes.h"

#define FIFO_ALGORITHM_READ "/tmp/fifo_algoritm_write"

//gcc -Wall -Wextra -std=c11 -g testreader.c ../Pipes.c -I../../ -o testreader


int main () {

    InputCache_t *cache = malloc(sizeof(InputCache_t));
    if (!cache) {
        fprintf(stderr, "Failed to allocate memory for InputCache\n");
        return -1;
    }
    memset(cache, 0, sizeof(InputCache_t));

    int fifo_fd = open(FIFO_ALGORITHM_READ, O_RDONLY);

    ssize_t bytes_read = Pipes_ReadBinary(fifo_fd, cache, sizeof(InputCache_t));

    close(fifo_fd);
    unlink(FIFO_ALGORITHM_READ);

    if (bytes_read != sizeof(InputCache_t)) {
        fprintf(stderr, "Failed to read complete data (got %zd, expected %zu bytes)\n",
                bytes_read, sizeof(InputCache_t));
        return -1;
    }
    printf("Received from cache Meteo: %zu HomeSystem: %zu price areas: %zu \n", cache->meteo_count, cache->home_count, sizeof(cache->spotpris.areas) / sizeof(cache->spotpris.areas[0]));

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

    for (size_t area_idx = 0; area_idx < 4; area_idx++) {
        printf("\n=== Area %s ===\n", cache->spotpris.areas[area_idx].areaname);
        printf("Total kvartar: %zu\n", cache->spotpris.areas[area_idx].count);
    
        size_t show_count = cache->spotpris.areas[area_idx].count;
        if (show_count > 10) show_count = 10;  // Show only first 10
        
        for (size_t j = 0; j < show_count; j++) {
            printf("  [%2zu] %s -> %.5f SEK/kWh\n", 
                   j,
                   cache->spotpris.areas[area_idx].kvartar[j].time_start,
                   cache->spotpris.areas[area_idx].kvartar[j].sek_per_kwh);
        }
    
        if (cache->spotpris.areas[area_idx].count > 10) {
            printf("  ... (%zu more entries)\n", 
           cache->spotpris.areas[area_idx].count - 10);
        }
    }

    printf("Free cache\n");
    free(cache);
}