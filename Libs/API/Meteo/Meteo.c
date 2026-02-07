#include "Meteo.h"
#include "../../Fetcher.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <jansson.h>


int meteo_Fetch(double lat, double lon, MeteoData *weather_out, int max_hours)
{
    if (!weather_out || max_hours <= 0 || max_hours > 72)
        return -1;

    //char key[64];
    //snprintf(key, sizeof(key), "%.2f_%.2f", lat, lon);

    
    
    char url[512];
    snprintf(url, sizeof(url),
             "https://api.open-meteo.com/v1/forecast?"
             "latitude=%.2f&longitude=%.2f"
             "&hourly=temperature_2m,shortwave_radiation,direct_normal_irradiance,diffuse_radiation,cloud_cover,is_day"
             "&forecast_days=1&timezone=Europe/Stockholm",
             lat, lon);
    
    CurlResponse *response = (CurlResponse *)malloc(sizeof(CurlResponse));
    if (!response) return -1;
    
    response->data = NULL;
    response->size = 0;
    
    // Use Fetcher to get the data
    int fetch_result = Curl_HTTPGet(response, url);
    if (fetch_result != 0 || response->data == NULL) {
        fprintf(stderr, "[METEO] HTTP fetch failed with code %d\n", fetch_result);
        Curl_Dispose(response);
        return -1;
    }
        
    json_error_t error;
    struct json_t *root = json_loads(response->data, 0, &error);
    if (!root)
    {
        fprintf(stderr, "[METEO] JSON parse failed: %s\n", error.text);
        Curl_Dispose(response);
        return -1;
    }
    
    json_t *hourly = json_object_get(root, "hourly");
    if (!hourly || !json_is_object(hourly))
    {
        fprintf(stderr, "[METEO] JSON missing 'hourly' object\n");
        json_decref(root);
        Curl_Dispose(response);
        return -1;
    }

    json_t *temps = json_object_get(hourly, "temperature_2m");
    json_t *ghi = json_object_get(hourly, "shortwave_radiation");
    json_t *dni = json_object_get(hourly, "direct_normal_irradiance");
    json_t *diffuse = json_object_get(hourly, "diffuse_radiation");
    json_t *cloud_cover = json_object_get(hourly, "cloud_cover");
    json_t *is_day = json_object_get(hourly, "is_day");

    int hours = json_array_size(temps);
    if (hours > max_hours)
        hours = max_hours;
    
    for (int i = 0; i < hours; i++)
    {
        weather_out[i].temp = json_number_value(json_array_get(temps, i));
        weather_out[i].ghi = json_number_value(json_array_get(ghi, i));
        weather_out[i].dni = json_number_value(json_array_get(dni, i));
        weather_out[i].diffuse_radiation = json_number_value(json_array_get(diffuse, i));
        weather_out[i].cloud_cover = json_number_value(json_array_get(cloud_cover, i));
        weather_out[i].is_day = json_integer_value(json_array_get(is_day, i)) != 0;
        weather_out[i].valid = weather_out[i].is_day;
    }
    
    json_decref(root);
    Curl_Dispose(response);
    return hours;
}

int meteo_SaveToFile(const char *data, double lat, double lon)          //uppdatera cache för att ha samma SaveToFile för all cache?
{
    if (!data) return -1;

    //// Round coordinates to 0.1 degree (~11km grid)
    //double rounded_lat = round(lat * 10.0) / 10.0;          //klurigt?
    //double rounded_lon = round(lon * 10.0) / 10.0;

    char filename[128];

    //MUTEXLOCK??
    snprintf(filename, sizeof(filename), "cache_meteo/meteo_%.2f_%.2f_.json",
             lat, lon);

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "[METEO] Failed to open %s for writing\n", filename);
        return -1;
    }

    fputs(data, fp);        //need better
    fclose(fp);
    //MUTEXUNLOCK??

    printf("[METEO] Saved full response to: meteo_%.2f_%.2f_.json\n",
           lat, lon);
    return 0;
}
