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
DagligSpotpris *spotpris_data;
size_t spotpris_count;
} Datacollector;

//gcc -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200112L -ILibs -ILibs/API -ILibs/Cache -ILibs/Utils Tests/structmeteospotpris.c Libs/Fetcher.c Libs/API/Meteo.c Libs/API/Spotpris.c Libs/Cache/Cache.c -lcurl -ljansson -o Tests/structmeteospotpris

int main()
{
    Datacollector *collecteddata = malloc(sizeof(Datacollector));
    if (!collecteddata) {
        fprintf(stderr, "Failed to allocate Datacollector\n");
        return -1;
    }

    // Allocate meteo data array (288 intervals)
    collecteddata->meteo_data = malloc(288 * sizeof(MeteoData));
    if (!collecteddata->meteo_data) {
        fprintf(stderr, "Failed to allocate meteo_data\n");
        free(collecteddata);
        return -1;
    }

    // Allocate spotpris data
    collecteddata->spotpris_data = malloc(sizeof(DagligSpotpris));
    if (!collecteddata->spotpris_data) {
        fprintf(stderr, "Failed to allocate spotpris_data\n");
        free(collecteddata->meteo_data);
        free(collecteddata);
        return -1;
    }
    collecteddata->spotpris_data->entries = NULL; // Will be allocated by Spotpris_Fetch

    // Fetch 288 intervals (3 days × 96)
    int meteo_count = meteo_Fetch(57.70, 12.00, collecteddata->meteo_data, 288);
    printf("Fetched %d meteo intervals\n", meteo_count);

    // Fetch spot price - still need multi-day version!
    Spotpris_Fetch(collecteddata->spotpris_data, "SE1");
    printf("Fetched %zu price intervals\n", collecteddata->spotpris_data->count);


    for (size_t i = 0; i < 288; i++)
    {
        //printf("time: %s, price SEK/kWh: %.2f\n", collecteddata->spotpris_data->entries[i].time_start, collecteddata->spotpris_data->entries[i].sek_per_kwh);
        printf("time: %s, temp: %.2f C, GHI: %.2f W/m²\n", collecteddata->meteo_data[i].time, collecteddata->meteo_data[i].temp, collecteddata->meteo_data[i].ghi);
    }

    // Cleanup
    if (collecteddata->spotpris_data->entries) {
        free(collecteddata->spotpris_data->entries);
    }
    free(collecteddata->spotpris_data);
    free(collecteddata->meteo_data);
    free(collecteddata);

    return 0;
}