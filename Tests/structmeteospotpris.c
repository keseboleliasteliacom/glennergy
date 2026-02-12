#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Libs/Fetcher.h"
#include "../Libs/API/Meteo.h"
#include "../Libs/Fetcher.h"
#include "../Libs/API/Spotpris.h"

typedef struct {
    MeteoData *meteo_data;
    size_t meteo_count;
    AllaSpotpriser *spotpris_data;
} Datacollector;

//gcc -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200112L -D_GNU_SOURCE -ILibs -ILibs/API -ILibs/Cache -ILibs/Utils Tests/structmeteospotpris.c Libs/Fetcher.c Libs/API/Meteo.c Libs/API/Spotpris.c Libs/Cache/Cache.c -lcurl -ljansson -o Tests/structmeteospotpris

int main()
{
    Datacollector *collecteddata = malloc(sizeof(Datacollector));
    if (!collecteddata) {
        fprintf(stderr, "Failed to allocate Datacollector\n");
        return -1;
    }
    
    // Allocate meteo data array (288 intervals = 3 days × 96)
    collecteddata->meteo_data = malloc(288 * sizeof(MeteoData));
    if (!collecteddata->meteo_data) {
        fprintf(stderr, "Failed to allocate meteo_data\n");
        free(collecteddata);
        return -1;
    }
    
    // Allocate spotpris data (all zones in one struct!)
    collecteddata->spotpris_data = malloc(sizeof(AllaSpotpriser));
    if (!collecteddata->spotpris_data) {
        fprintf(stderr, "Failed to allocate spotpris_data\n");
        free(collecteddata->meteo_data);
        free(collecteddata);
        return -1;
    }
    
    // Fetch meteo 288 intervals (3 days × 96)
    int meteo_count = meteo_Fetch(57.70, 12.00, collecteddata->meteo_data, 288);
    printf("Fetched %d meteo intervals\n", meteo_count);

    // Fetch spot price for one zone (SE3)
    // int spotpris_result = Spotpris_FetchArea(collecteddata->spotpris_data, AREA_SE3);
    // if (spotpris_result == 0) {
    //     printf("Fetched %zu spotpris intervals for SE3\n", 
    //            collecteddata->spotpris_data->num_intervals[AREA_SE3]);
    // } else {
    //     fprintf(stderr, "Failed to fetch spotpris: %d\n", spotpris_result);
    // }
    // // Print first 10 intervals to verify alignment
    // printf("\n=== First 10 spotpris intervals (SE3) ===\n");
    // for (size_t i = 0; i < 10 && i < collecteddata->spotpris_data->num_intervals[AREA_SE3]; i++)
    // {
    //     printf("[%3zu] Spotpris time: %s, Price: %.2f SEK/kWh\n", 
    //            i, 
    //            collecteddata->spotpris_data->data[AREA_SE3][i].time_start, 
    //            collecteddata->spotpris_data->data[AREA_SE3][i].sek_per_kwh);
    // }
    int spotpris_result = Spotpris_FetchAll(collecteddata->spotpris_data);

    printf("\n=== First 10 meteo intervals ===\n");
    for (size_t i = 0; i < 10 && i < (size_t)meteo_count; i++) {
        printf("[%3zu] Meteo time: %s, Temp: %.2f°C, GHI: %.2f W/m²\n",
               i,
               collecteddata->meteo_data[i].time, 
               collecteddata->meteo_data[i].temp, 
               collecteddata->meteo_data[i].ghi);
    }

    printf("\n=== first 10 spotpris intervals (SE1-SE4) ===\n");
    for (size_t area = 0; area < AREA_COUNT; area++)
    {
        printf("Area: %s\n", area_to_string((SpotprisArea)area));
        for (size_t i = 0; i < 10 && i < collecteddata->spotpris_data->num_intervals[area]; i++)
        {
            printf("[%3zu] Spotpris time: %s, Price: %.2f SEK/kWh\n", 
                   i, 
                   collecteddata->spotpris_data->data[area][i].time_start, 
                   collecteddata->spotpris_data->data[area][i].sek_per_kwh);
        }
        printf("\n");
    }

    // Cleanup - just 2 frees now!
    free(collecteddata->spotpris_data);
    free(collecteddata->meteo_data);
    free(collecteddata);

    return 0;
}