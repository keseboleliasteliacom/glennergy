#include "Meteo.h"
#include "../Fetcher.h"
#include "../Cache/Cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <jansson.h>

#define METEO_LINK "https://api.open-meteo.com/v1/forecast?latitude=%.2f&longitude=%.2f&minutely_15=temperature_2m,shortwave_radiation&past_days=1&forecast_days=2&timezone=Europe/Stockholm"

static Cache meteo_cache;
static bool cache_initialized = false;


int meteo_ParseJSON(MeteoData *meteo_out, int max_hours, const char *json_str)
{
    if (!meteo_out || max_hours <= 0 || max_hours > 288 || !json_str)
        return -1;

    json_error_t error;
    struct json_t *root = json_loads(json_str, 0, &error);
    if (!root)
    {
        fprintf(stderr, "[METEO] JSON parse failed: %s\n", error.text);
        return -1;
    }
    
    json_t *minutely = json_object_get(root, "minutely_15");
    if (!minutely || !json_is_object(minutely))
    {
        fprintf(stderr, "[METEO] JSON missing 'minutely_15' object\n");
        json_decref(root);
        return -1;
    }
    json_t *times = json_object_get(minutely, "time");
    json_t *temps = json_object_get(minutely, "temperature_2m");
    json_t *ghi = json_object_get(minutely, "shortwave_radiation");
    //json_t *dni = json_object_get(minutely, "direct_normal_irradiance");
    //json_t *diffuse = json_object_get(minutely, "diffuse_radiation");
    //json_t *cloud_cover = json_object_get(minutely, "cloud_cover");
    //json_t *is_day = json_object_get(minutely, "is_day");


    int hours = json_array_size(temps);
    if (hours > max_hours)
        hours = max_hours;
    
    for (int i = 0; i < hours; i++)
    {
        const char *time_str = json_string_value(json_array_get(times, i));
        if (time_str)
        {
        strncpy(meteo_out[i].time, time_str, sizeof(meteo_out[i].time) - 1);
        meteo_out[i].time[sizeof(meteo_out[i].time) - 1] = '\0';
        }
        meteo_out[i].temp = json_number_value(json_array_get(temps, i));
        meteo_out[i].ghi = json_number_value(json_array_get(ghi, i));
        //meteo_out[i].dni = json_number_value(json_array_get(dni, i));
        //meteo_out[i].diffuse_radiation = json_number_value(json_array_get(diffuse, i));
        //meteo_out[i].cloud_cover = json_number_value(json_array_get(cloud_cover, i));
        //meteo_out[i].is_day = json_integer_value(json_array_get(is_day, i)) != 0;
        //meteo_out[i].valid = meteo_out[i].is_day;
    }
    
    json_decref(root); // still need to free the root object
    return hours;
}

int meteo_Fetch(double lat, double lon, MeteoData *meteo_out, int max_hours)
{
    if (!meteo_out || max_hours <= 0 || max_hours > 288)
        return -1;

    // Initialize cache once
    if (!cache_initialized) {
        if (cache_Init(&meteo_cache, "cache_meteo") == 0) {
            cache_initialized = true;
        }
    }

    char url[512];
    snprintf(url, sizeof(url), METEO_LINK, lat, lon);
    
    CurlResponse *response = (CurlResponse *)malloc(sizeof(CurlResponse));
    if (!response) return -1;
    
    response->data = NULL;
    response->size = 0;
    
    int fetch_result = Curl_HTTPGet(response, url);
    if (fetch_result != 0 || response->data == NULL) {
        fprintf(stderr, "[METEO] HTTP fetch failed with code %d\n", fetch_result);
        Curl_Dispose(response);
        return -1;
    }

    // Save to cache with key: "lat_lon_date"
    if (cache_initialized) {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        char key[128];
        snprintf(key, sizeof(key), "%.2f_%.2f_%04d-%02d-%02d",
                 lat, lon, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
        cache_Set(&meteo_cache, key, response->data, response->size, 3600); // 1 hour TTL
    }

    int hours = meteo_ParseJSON(meteo_out, max_hours, response->data);
    if (hours < 0) {
        fprintf(stderr, "[METEO] Failed to parse meteo data\n");
        Curl_Dispose(response);
        return -1;
    }

    Curl_Dispose(response);
    return hours;
}
