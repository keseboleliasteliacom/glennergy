#include "InputCache.h"
#include "../Libs/Utils/utils.h"
#include <stdio.h>
#include <time.h>
#include <jansson.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#define FIFO_METEO_READ "/tmp/fifo_meteo_read"
#define FIFO_SPOTPRIS_READ "/tmp/fifo_spotpris"
#define FIFO_ALGORITHM_WRITE "/tmp/fifo_algoritm_write"

static void GetTodayDate(char *buffer, size_t size)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buffer, size, "%04d/%02d-%02d",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

// För fil: YYYY-MM-DD (Filnamn får inte ha snestreck "/")
static void GetTodayDateFile(char *buffer, size_t size)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buffer, size, "%04d-%02d-%02d",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

int InputCache_SaveSpotpris(const AllaSpotpriser *spotpris)
{
    if (!spotpris)
        return -1;

    int result = 0;
    // Finns cache-foldern?

    const char *cache_folder = "../cache_spotpris";
    dir_result_t dir_res = create_folder(cache_folder);
    if (dir_res == DIR_ERROR)
    {
        fprintf(stderr, "Error creating cache folder: %s\n", cache_folder);
        return -2;
    }

    char filename[64];
    char date_str[16];
    GetTodayDateFile(date_str, sizeof(date_str));

    for (int i = 0; i < 4; i++)
    {
        snprintf(filename, sizeof(filename), "%s/spotpris_%s_%s.json", cache_folder, spotpris->areas[i].areaname, date_str);

        json_t *root = json_array();

        for (size_t j = 0; j < 96; j++)
        {
            json_t *obj = json_object();
            json_object_set_new(obj, "time_start", json_string(spotpris->areas[i].kvartar[j].time_start));
            json_object_set_new(obj, "time_end", json_string(spotpris->areas[i].kvartar[j].time_end));
            json_object_set_new(obj, "SEK_per_kWh", json_real(spotpris->areas[i].kvartar[j].sek_per_kwh));
            json_object_set_new(obj, "EUR_per_kWh", json_real(spotpris->areas[i].kvartar[j].eur_per_kwh));
            json_object_set_new(obj, "EXR", json_real(spotpris->areas[i].kvartar[j].exchange_rate));
            json_array_append_new(root, obj);
        }

        result = json_dump_file(root, filename, JSON_INDENT(4));

        if (result < 0)
        {
            printf("Failed to dump save file for spotpris area: %s", spotpris->areas[i].areaname);
        }

        json_decref(root);
    }

    // printf("Saving %zu entries to file: spotpris_%s_%s.json\n", spotpris->count, spotpris->areaname, date_str);

    return 0;
}

int InputCache_SaveMeteo(const MeteoData *_Data)
{
    if (!_Data)
        return -1;

    int result = 0;

    const char *meteo_folder = "../cache_meteo";
    dir_result_t dir_res = create_folder(meteo_folder);
    if (dir_res == DIR_ERROR)
    {
        fprintf(stderr, "Error creating cache folder: %s\n", meteo_folder);
        return -2;
    }

    char filename[64];
    char date_str[16];
    GetTodayDateFile(date_str, sizeof(date_str));

    for (int i = 0; i < _Data->pCount; i++)
    {
        snprintf(filename, sizeof(filename), "%s/meteo_%d_%s.json", meteo_folder, _Data->pInfo[i].id, date_str);
        json_error_t error;
        json_t *root = json_loads(_Data->pInfo[i].raw_json_data, 0, &error);

        if (root == NULL)
        {
            printf("Failed to load raw json [METEO]\n");
            return -2;
        }
        result = json_dump_file(root, filename, JSON_INDENT(4));

        if (result < 0)
        {
            printf("Failed to dump save file for meteo, property id: %d", _Data->pInfo[i].id);
        }
        json_decref(root);
    }

    // printf("Saving %zu entries to file: spotpris_%s_%s.json\n", spotpris->count, spotpris->areaname, date_str);

    return 0;
}