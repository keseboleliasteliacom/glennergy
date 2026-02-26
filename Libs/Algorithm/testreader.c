#define MODULE_NAME "TESTREADER"
#include "../../Server/Log/Logger.h"
#include "testreader.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "average.h"

#include "../../Cache/InputCache.h"
#include "../Pipes.h"

#define FIFO_ALGORITHM_READ "/tmp/fifo_algoritm"

// gcc -Wall -Wextra -std=c11 -g testreader.c ../Pipes.c -I../../ -o testreader

int test_reader()
{
    char *time_stamps[] = {
        "%s",
        "%s",
        "%s",
        "%s",
        "2024-06-01T01:00:00Z"};
    InputCache_t *cache = malloc(sizeof(InputCache_t));
    if (!cache)
    {
        LOG_ERROR("malloc() Failed to allocate memory for InputCache");
        return -1;
    }

    while (1)
    {
        memset(cache, 0, sizeof(InputCache_t));

        int fifo_fd = open(FIFO_ALGORITHM_READ, O_RDONLY);

        if (fifo_fd < 0)
        {
            LOG_ERROR("Failed to open FIFO for reading: %s", FIFO_ALGORITHM_READ);
            free(cache);
            return -1;
        }

        ssize_t bytes_read = Pipes_ReadBinary(fifo_fd, cache, sizeof(InputCache_t));

        close(fifo_fd);
        // unlink(FIFO_ALGORITHM_READ);

        if (bytes_read != sizeof(InputCache_t))
        {
            LOG_ERROR("Failed to read complete data (got %zd, expected %zu bytes)", bytes_read, sizeof(InputCache_t));
            return -1;
        }
        LOG_INFO("Received from cache Meteo: %zu HomeSystem: %zu price areas: %zu", cache->meteo_count, cache->home_count, sizeof(cache->spotpris.count) / sizeof(cache->spotpris.count[0]));

        char time_buffer[32];
        snprintf(time_buffer, sizeof(time_buffer), cache->meteo[0].sample[0].time_start);
        char *hour = time_buffer + 11;   // Move pointer to the hour part
        hour[2] = '\0';                  // Null-terminate after the hour
        char *minute = time_buffer + 14; // Move pointer to the minute part
        minute[2] = '\0';                // Null-terminate after the minute

        for (size_t i = 0; i < cache->meteo_count; i++)
        {
            for (size_t j = 0; j < 2 && j < KVARTAR_TOTALT; j++)
            {
                printf("time: %s, temp: %.2f\n",
                       cache->meteo[i].sample[j].time_start,
                       cache->meteo[i].sample[j].temp);
            }
        }

        const char *area_names[AREA_COUNT] = {"SE1", "SE2", "SE3", "SE4"};

        size_t spot_index = 0;

        SpotStats_t stats;
        average_SpotprisStats(&stats, cache);

        for (size_t area_idx = 0; area_idx < 4; area_idx++)
        {

            size_t show_count = cache->spotpris.count[area_idx];
            // if (show_count > 10)
            //   show_count = 10; // Show only first 10

            for (size_t entry = 0; entry < show_count; entry++)
            {
                char time_buffer_spot[32];
                snprintf(time_buffer_spot, sizeof(time_buffer_spot), cache->spotpris.data[area_idx][entry].time_start);
                char *hour_spot = time_buffer_spot + 11;   // Move pointer to the hour part
                hour_spot[2] = '\0';                       // Null-terminate after the hour
                char *minute_spot = time_buffer_spot + 14; // Move pointer to the minute part
                minute_spot[2] = '\0';                     // Null-terminate after the minute

                if (strncmp(hour, hour_spot, 2) == 0 && strncmp(minute, minute_spot, 2) == 0)
                {
                    spot_index = entry; // Store the index of the matching spotpris entry
                }
            }

            for (size_t i = 0; i < cache->meteo_count; i++)
            {

                printf("Comparing meteo area '%s' with spotpris area '%s'\n", cache->meteo[i].electricity_area, area_names[area_idx]);
                if (strncmp(cache->meteo[i].electricity_area, area_names[area_idx], 3) == 0)
                {
                    for (size_t entry = spot_index; entry < show_count; entry++)
                    {
                        char time_buffer_spot[32];
                        snprintf(time_buffer_spot, sizeof(time_buffer_spot), cache->spotpris.data[area_idx][entry].time_start);
                        char *hour_spot = (char *)time_buffer_spot + 11;   // Move pointer to the hour part
                        hour_spot[2] = '\0';                               // Null-terminate after the hour
                        char *minute_spot = (char *)time_buffer_spot + 14; // Move pointer to the minute part
                        minute_spot[2] = '\0';


                        printf("Area: %s, time: %s, price: %.2f SEK/kWh,\n",
                               area_names[area_idx],
                               cache->spotpris.data[area_idx][entry].time_start,
                               cache->spotpris.data[area_idx][entry].sek_per_kwh);
                        for (size_t j = 0; j < 96; j++)
                        {

                            char time_buffer[32];
                            snprintf(time_buffer, sizeof(time_buffer), cache->meteo[i].sample[j].time_start);
                            char *hour = time_buffer + 11;   // Move pointer to the hour part
                            hour[2] = '\0';                  // Null-terminate after the hour
                            char *minute = time_buffer + 14; // Move pointer to the minute part
                            minute[2] = '\0';                // Null-terminate after the minute

                            if (strncmp(hour, hour_spot, 2) == 0 && strncmp(minute, minute_spot, 2) == 0)
                            {
                                average_WindowLow_test(&cache->spotpris.data[area_idx][entry], stats.area[area_idx].q25, stats.area[area_idx].q75);
                                printf("  Matched time: %s, temp: %.2f °C, GHI: %.2f W/m², City: %s\n",
                                       cache->meteo[i].sample[j].time_start,
                                       cache->meteo[i].sample[j].temp,
                                       cache->meteo[i].sample[j].ghi,
                                       cache->meteo[i].city);


                            }
                        }
                    }
                }
            }
        }

        

        sleep(10);
    } // while
    printf("Free cache\n");
    free(cache);
    return 0;
}