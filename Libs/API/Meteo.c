/*#include "Meteo.h"
#include "../Fetcher.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <json-c/json.h>

int meteo_Fetch(double lat, double lon, WeatherData *weather_out, int max_hours)
{
    if (!weather_out || max_hours <= 0 || max_hours > 72)
        return -1;
    
    char url[512];
    snprintf(url, sizeof(url),
             "https://api.open-meteo.com/v1/forecast?"
             "latitude=%.4f&longitude=%.4f"
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
    
    struct json_object *root = json_tokener_parse(response->data);
    
    if (!root)
    {
        fprintf(stderr, "[METEO] JSON parse failed\n");
        Curl_Dispose(response);
        return -1;
    }
    
    struct json_object *hourly, *temps, *ghi, *dni, *diffuse, *cloud_cover, *is_day;
    if (!json_object_object_get_ex(root, "hourly", &hourly))
    {
        json_object_put(root);
        Curl_Dispose(response);
        return -1;
    }
    
    json_object_object_get_ex(hourly, "temperature_2m", &temps);
    json_object_object_get_ex(hourly, "shortwave_radiation", &ghi);
    json_object_object_get_ex(hourly, "direct_normal_irradiance", &dni);
    json_object_object_get_ex(hourly, "diffuse_radiation", &diffuse);
    json_object_object_get_ex(hourly, "cloud_cover", &cloud_cover);
    json_object_object_get_ex(hourly, "is_day", &is_day);

    int count = json_object_array_length(temps);
    if (count > max_hours)
        count = max_hours;
    
    for (int i = 0; i < count; i++)
    {
        weather_out[i].temp = json_object_get_double(json_object_array_get_idx(temps, i));
        weather_out[i].ghi = json_object_get_double(json_object_array_get_idx(ghi, i));
        weather_out[i].dni = json_object_get_double(json_object_array_get_idx(dni, i));
        weather_out[i].diffuse_radiation = json_object_get_double(json_object_array_get_idx(diffuse, i));
        weather_out[i].cloud_cover = json_object_get_double(json_object_array_get_idx(cloud_cover, i));
        weather_out[i].is_day = json_object_get_int(json_object_array_get_idx(is_day, i)) != 0;
        weather_out[i].valid = weather_out[i].is_day;
    }
    
    json_object_put(root);
    Curl_Dispose(response);
    return count;
}*/
