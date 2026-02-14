#include "spotpris.h"
#include "../utils/fetcher.h"
#include "../cache/cache.h"
#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../utils/utils.h"

static Cache spotpris_cache;
static bool cache_initialized = false;

// Hämta dagens datum i formatet YYYY/MM-DD (URLer måste ha YYYY/MM-DD format)
static void GetTodayDate(char *buffer, size_t size) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buffer, size, "%04d/%02d-%02d",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

// För fil: YYYY-MM-DD (Filnamn får inte ha snestreck "/")
static void GetTodayDateFile(char *buffer, size_t size) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buffer, size, "%04d-%02d-%02d",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

/*
// OLD IMPLEMENTATION - Kept for reference but not used
int Spotpris_Fetch(DagligSpotpris *spotpris, const char *area)
{
    if (!spotpris || !area) return -1;

    char date_str[16];
    GetTodayDate(date_str, sizeof(date_str));

    char url[256];
    snprintf(url, sizeof(url),
             "https://www.elprisetjustnu.se/api/v1/prices/%s_%s.json",
             date_str, area);

    CurlResponse resp;
    Curl_Initialize(&resp);

    int rc = Curl_HTTPGet(&resp, url);
    if (rc != 0) {
        Curl_Dispose(&resp);
        return rc;
    }
    // Parse JSON
    json_error_t error;
    json_t *root = json_loads(resp.data, 0, &error);
    Curl_Dispose(&resp);
    if (!root) {
        fprintf(stderr, "JSON parse error: %s\n", error.text);
        return -2;
    }

    if (!json_is_array(root)) {
        json_decref(root);
        return -3;
    }

    size_t n = json_array_size(root);
    spotpris->entries = malloc(sizeof(SpotPriceEntry) * n);
    if (!spotpris->entries) {
        json_decref(root);
        return -4;
    }

    // GLöm inte fixa nullterminering
    strncpy(spotpris->area, area, sizeof(spotpris->area)-1);
    spotpris->area[sizeof(spotpris->area)-1] = '\0';
    spotpris->count = n;

    for (size_t i = 0; i < n; i++) {
        json_t *obj = json_array_get(root, i);

        const char *start = json_string_value(json_object_get(obj, "time_start"));
        //const char *end   = json_string_value(json_object_get(obj, "time_end"));
        spotpris->entries[i].sek_per_kwh = json_real_value(json_object_get(obj, "SEK_per_kWh"));
        //spotpris->entries[i].eur_per_kwh = json_real_value(json_object_get(obj, "EUR_per_kWh"));
        //spotpris->entries[i].exchange_rate = json_real_value(json_object_get(obj, "EXR"));

        // Glöm inte nullterminatorn här också
        strncpy(spotpris->entries[i].time_start, start, sizeof(spotpris->entries[i].time_start));
        spotpris->entries[i].time_start[sizeof(spotpris->entries[i].time_start)-1] = '\0';

        //strncpy(spotpris->entries[i].time_end, end, sizeof(spotpris->entries[i].time_end));
        //spotpris->entries[i].time_end[sizeof(spotpris->entries[i].time_end)-1] = '\0';
    }

    json_decref(root);
    return 0;
}

// Exempel på sparad fil: spotpris_SE1_2026-01-26.json
int Spotpris_SaveToFile(const DagligSpotpris *spotpris)
{
    if (!spotpris || !spotpris->entries) return -1;

    // Finns cache-foldern?
    const char *cache_folder = "../cache_spotpris";
    dir_result_t dir_res = create_folder(cache_folder);
    if (dir_res == DIR_ERROR) {
        fprintf(stderr, "Error creating cache folder: %s\n", cache_folder);
        return -2;
    }

    char filename[64];
    char date_str[16];
    GetTodayDateFile(date_str, sizeof(date_str));
    snprintf(filename, sizeof(filename), "%s/spotpris_%s_%s.json", cache_folder, spotpris->area, date_str);

    json_t *root = json_array();
    for (size_t i = 0; i < spotpris->count; i++) {
        json_t *obj = json_object();
        json_object_set_new(obj, "time_start", json_string(spotpris->entries[i].time_start));
        //json_object_set_new(obj, "time_end",   json_string(spotpris->entries[i].time_end));
        json_object_set_new(obj, "SEK_per_kWh", json_real(spotpris->entries[i].sek_per_kwh));
        //json_object_set_new(obj, "EUR_per_kWh", json_real(spotpris->entries[i].eur_per_kwh));
        //json_object_set_new(obj, "EXR",        json_real(spotpris->entries[i].exchange_rate));
        json_array_append_new(root, obj);
    }
    printf("Saving %zu entries to file: spotpris_%s_%s.json\n", spotpris->count, spotpris->area, date_str);


    int ret = json_dump_file(root, filename, JSON_INDENT(4));
    json_decref(root);
    return ret;
}
*/

const char* area_to_string(SpotprisArea area) {
    switch(area) {
        case AREA_SE1: return "SE1";
        case AREA_SE2: return "SE2";
        case AREA_SE3: return "SE3";
        case AREA_SE4: return "SE4";
        default: return "UNKNOWN";
    }
}

SpotprisArea string_to_area(const char *str) {
    if (strcmp(str, "SE1") == 0) return AREA_SE1;
    if (strcmp(str, "SE2") == 0) return AREA_SE2;
    if (strcmp(str, "SE3") == 0) return AREA_SE3;
    if (strcmp(str, "SE4") == 0) return AREA_SE4;
    return AREA_SE1; // default
}

int Spotpris_FetchArea(AllaSpotpriser *spotpris, SpotprisArea area)
{
    if (!spotpris) return -1;

    spotpris->num_intervals[area] = 0;
    const char *area_str = area_to_string(area);

    // Fetch Yesterday, today, tomorrow (day -1, 0, +1)
    for (int day_offset = -1; day_offset < 2; day_offset++)
    {
        time_t t = time(NULL) + (day_offset * 86400);
        struct tm tm = *localtime(&t);
        
        char date_str[32];
        snprintf(date_str, sizeof(date_str), "%04d/%02d-%02d",
                 tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

        char url[256];
        snprintf(url, sizeof(url),
                 "https://www.elprisetjustnu.se/api/v1/prices/%s_%s.json",
                 date_str, area_str);

        printf("Fetching %s for %s (day %+d)\n", area_str, date_str, day_offset);

        CurlResponse resp;
        Curl_Initialize(&resp);

        int rc = Curl_HTTPGet(&resp, url);
        if (rc != 0) {
            fprintf(stderr, "Failed to fetch %s day %+d: %d\n", area_str, day_offset, rc);
            Curl_Dispose(&resp);
            if (day_offset == -1) return rc;  // Must have at least yesterday
            break;
        }

        // Save to cache with key: "area_date"
        if (!cache_initialized) {
            if (cache_Init(&spotpris_cache, "data/cache_spotpris") == 0) {
                cache_initialized = true;
            }
        }
        if (cache_initialized) {
            char key[128];
            snprintf(key, sizeof(key), "%s_%04d-%02d-%02d",
                     area_str, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
            cache_Set(&spotpris_cache, key, resp.data, resp.size, 86400); // 24 hour TTL
        }

        json_error_t error;
        json_t *root = json_loads(resp.data, 0, &error);
        Curl_Dispose(&resp);
        
        if (!root) {
            fprintf(stderr, "JSON parse error %s day %+d: %s\n", area_str, day_offset, error.text);
            if (day_offset == -1) return -2;
            break;
        }

        if (!json_is_array(root)) {
            json_decref(root);
            if (day_offset == -1) return -3;
            break;
        }

        size_t n_elements = json_array_size(root);
        
        
        if (spotpris->num_intervals[area] + n_elements > SPOTPRIS_ENTRIES) {
            n_elements = SPOTPRIS_ENTRIES - spotpris->num_intervals[area];
        }

        for (size_t i = 0; i < n_elements; i++)
        {
            json_t *obj = json_array_get(root, i);
            size_t idx = spotpris->num_intervals[area] + i;

            const char *start = json_string_value(json_object_get(obj, "time_start"));
            spotpris->data[area][idx].sek_per_kwh = json_real_value(json_object_get(obj, "SEK_per_kWh"));

            strncpy(spotpris->data[area][idx].time_start, start, 
                    sizeof(spotpris->data[area][idx].time_start) - 1);
            spotpris->data[area][idx].time_start[sizeof(spotpris->data[area][idx].time_start)-1] = '\0';
        }

        spotpris->num_intervals[area] += n_elements;
        json_decref(root);
        
        printf("Loaded %zu intervals for %s day %+d (total: %zu)\n", 
               n_elements, area_str, day_offset, spotpris->num_intervals[area]);
    }

    return 0;
}

int Spotpris_FetchAll(AllaSpotpriser *spotpris)
{
    if (!spotpris) return -1;

    // Fetch all 4 areas
    for (int j = 0; j < AREA_COUNT; j++) {
        int result = Spotpris_FetchArea(spotpris, (SpotprisArea)j);
        if (result != 0) {
            fprintf(stderr, "Failed to fetch area %s\n", area_to_string((SpotprisArea)j));
            return result;
        }
    }

    return 0;
}