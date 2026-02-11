#include "../Libs/API/Meteo.h"
#include "../Libs/Fetcher.h"
#include "../Libs/Cache/Cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//test meteo with cache (run from Tests/ folder):
//gcc -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200112L -I../Libs -I../Libs/API -I../Libs/Cache -I../Libs/Utils main_meteotest.c ../Libs/API/Meteo.c ../Libs/Fetcher.c ../Libs/Cache/Cache.c -o meteo_test -lcurl -ljansson -lm -lpthread


int main()
{
    WeatherData weather[24];
    double lat = 57.70;
    double lon = 12.00;

    printf("=== Testing meteo_Fetch ===\n");
    int hours = meteo_Fetch(lat, lon, weather, 24);

    if (hours < 0)
    {
        printf("Error fetching weather data\n");
        return -1;
    }

    for (int i = 0; i < hours; i++)
    {
        if (weather[i].valid)
        {
            printf("hour %d: temp=%.2f: ghi=%.2f: dni=%.2f: diffuse=%.2f: cloud=%.2f: is_day=%d\n",
                   i,
                   weather[i].temp,
                   weather[i].ghi,
                   weather[i].dni,
                   weather[i].diffuse_radiation,
                   weather[i].cloud_cover,
                   weather[i].is_day);
        } 
    }
    
    printf("\n=== Testing Cache Integration ===\n");
    
    // Initialize cache
    Cache meteo_cache;
    if (cache_Init(&meteo_cache, "test_meteofolder") != 0) {
        printf("Error initializing cache\n");
        return -1;
    }
    
    // Fetch raw data
    char url[512];
    snprintf(url, sizeof(url),
             "https://api.open-meteo.com/v1/forecast?"
             "latitude=%.2f&longitude=%.2f"
             "&hourly=temperature_2m,shortwave_radiation,direct_normal_irradiance,diffuse_radiation,cloud_cover,is_day"
             "&forecast_days=1&timezone=Europe/Stockholm",
             lat, lon);
    
    CurlResponse *response = (CurlResponse *)malloc(sizeof(CurlResponse));
    if (!response) {
        printf("Error allocating response\n");
        cache_Dispose(&meteo_cache);
        return -1;
    }
    
    response->data = NULL;
    response->size = 0;
    
    if (Curl_HTTPGet(response, url) != 0 || response->data == NULL) {
        printf("Error fetching data for cache test\n");
        Curl_Dispose(response);
        cache_Dispose(&meteo_cache);
        return -1;
    }
    
    // Build cache key
    char key[64];
    snprintf(key, sizeof(key), "%.2f_%.2f", lat, lon);
    
    // Save to cache
    printf("Saving to cache with key: %s\n", key);
    if (cache_Set(&meteo_cache, key, response->data, response->size, 3600) == 0) {
        printf("Successfully saved to cache\n");
    } else {
        printf("Error saving to cache\n");
    }
    
    // Test cache retrieval
    char *cached_data = NULL;
    size_t cached_size = 0;
    
    printf("Retrieving from cache...\n");
    if (cache_Get(&meteo_cache, key, &cached_data, &cached_size)) {
        printf("Cache hit! Retrieved %zu bytes\n", cached_size);
        free(cached_data);
    } else {
        printf("Cache miss (unexpected!)\n");
    }
    
    Curl_Dispose(response);
    cache_Dispose(&meteo_cache);
    
    return 0;
}