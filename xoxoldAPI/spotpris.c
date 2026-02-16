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

int spotpris_ParseJSON(SpotPriceEntry *output, int max_entries, const char *json_str)
{
    if (!output || max_entries <= 0 || !json_str) {
        return -1;
    }

    json_error_t error;
    json_t *root = json_loads(json_str, 0, &error);
    
    if (!root) {
        fprintf(stderr, "[SPOTPRIS] JSON parse error: %s\n", error.text);
        return -1;
    }

    if (!json_is_array(root)) {
        fprintf(stderr, "[SPOTPRIS] JSON root is not an array\n");
        json_decref(root);
        return -1;
    }

    size_t n_elements = json_array_size(root);
    if (n_elements > (size_t)max_entries) {
        n_elements = max_entries;
    }

    for (size_t i = 0; i < n_elements; i++) {
        json_t *obj = json_array_get(root, i);
        
        const char *start = json_string_value(json_object_get(obj, "time_start"));
        //const char *end = json_string_value(json_object_get(obj, "time_end"));
        double sek = json_real_value(json_object_get(obj, "sek_per_kwh"));
        //double eur = json_real_value(json_object_get(obj, "EUR_per_kWh"));
        //double exr = json_real_value(json_object_get(obj, "EXR"));
        
        if (start) {
            strncpy(output[i].time_start, start, sizeof(output[i].time_start) - 1);
            output[i].time_start[sizeof(output[i].time_start) - 1] = '\0';
        }
        //if (end) {
        //    strncpy(output[i].time_end, end, sizeof(output[i].time_end) - 1);
        //    output[i].time_end[sizeof(output[i].time_end) - 1] = '\0';
        //}
        
        output[i].sek_per_kwh = sek;
        //output[i].eur_per_kwh = eur;
        //output[i].exchange_rate = exr;
    }

    json_decref(root);
    return (int)n_elements;
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

        int parsed = spotpris_ParseJSON( &spotpris->data[area][spotpris->num_intervals[area]],
                                            SPOTPRIS_ENTRIES - spotpris->num_intervals[area],
                                            resp.data
        );
        
        Curl_Dispose(&resp);
        
        if (parsed < 0) {
            fprintf(stderr, "Failed to parse JSON for %s day %+d\n", area_str, day_offset);
            if (day_offset == -1) return -2;
            break;
        }

        spotpris->num_intervals[area] += parsed;
        
        printf("Loaded %d intervals for %s day %+d (total: %zu)\n", 
               parsed, area_str, day_offset, spotpris->num_intervals[area]);
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