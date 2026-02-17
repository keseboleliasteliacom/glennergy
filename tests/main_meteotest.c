#include "../xoxoldAPI/meteo.h"
#include "../libs/utils/fetcher.h"
#include "../cache/cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//test meteo with cache (run from Tests/ folder):
//gcc -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L -I. -o tests/test_new_meteo tests/main_meteotest.c xoxoldAPI/meteo.c libs/utils/fetcher.c cache/cache.c server/log/logger.c -lcurl -ljansson -lm -lpthread

#define METEO_LINK "https://api.open-meteo.com/v1/forecast?latitude=%2.f&longitude=%2f&minutely_15=temperature_2m,shortwave_radiation&forecast_days=3&forecast_minutely_15=128"

//,direct_normal_irradiance,diffuse_radiation,cloud_cover,is_day
int main()
{
    double lat1 = 57.71;  // Gothenburg
    double lon1 = 12.20;
    
    printf("=== Testing meteo_Fetch (Fetch & Cache Only) ===\n");
    
    // Fetch and cache for property 1
    printf("Fetching weather for lat=%.2f, lon=%.2f...\n", lat1, lon1);
    int result = meteo_Fetch(lat1, lon1);
    
    if (result < 0) {
        printf("Error fetching weather data\n");
        return -1;
    }
    printf("Successfully fetched and cached\n\n");
    
    printf("=== Testing Cache Load & Parse ===\n");
    
    // Initialize cache
    Cache meteo_cache;
    if (cache_Init(&meteo_cache, "data/cache_meteo", 0) != 0) {
        printf("Error initializing cache\n");
        return -1;
    }
    
    // Build cache key (same as meteo_Fetch uses)
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char key[128];
    snprintf(key, sizeof(key), "%.2f_%.2f_%04d-%02d-%02d",
             lat1, lon1, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    
    printf("Loading from cache with key: %s\n", key);
    
    // Load from cache
    char *json_data = NULL;
    size_t size;
    
    if (!cache_Get(&meteo_cache, key, &json_data, &size)) {
        printf("Cache miss for %s\n", key);
        cache_Dispose(&meteo_cache);
        return -1;
    }
    
    printf("Loaded %zu bytes from cache\n", size);
    
    // Parse the cached JSON
    printf("\n=== Testing meteo_ParseJSON ===\n");
    MeteoData weather[288];  // Max 3 days * 96 (15-min intervals)
    int hours = meteo_ParseJSON(weather, 288, json_data);
    
    if (hours < 0) {
        printf("Error parsing weather data\n");
        free(json_data);
        cache_Dispose(&meteo_cache);
        return -1;
    }
    
    printf("Parsed %d data points\n\n", hours);
    
    // Display first 10 samples
    printf("First 10 samples:\n");
    for (int i = 0; i < 10 && i < hours; i++) {
        printf("  [%d] time=%s, temp=%.1f°C, ghi=%.1f W/m²\n",
               i, weather[i].time, weather[i].temp, weather[i].ghi);
    }
    
    printf("\n=== Test Complete ===\n");
    printf("Fetch → Cache → Load → Parse workflow working!\n");
    
    free(json_data);
    cache_Dispose(&meteo_cache);
    return 0;
}