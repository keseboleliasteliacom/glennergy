#include "../Libs/API/Meteo.h"
#include "../Libs/Fetcher.h"
#include <stdio.h>
#include <stdlib.h>

//test meteo (run from Tests/ folder):
//gcc -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200112L -I../Libs -I../Libs/API -I../Libs/Utils meteo_test.c ../Libs/API/Meteo.c ../Libs/Fetcher.c -o meteo_test -lcurl -ljansson -lm


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
    
    printf("\n=== Testing meteo_SaveToFile ===\n");
    
    // Fetch raw data to save to file
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
        return -1;
    }
    
    response->data = NULL;
    response->size = 0;
    
    if (Curl_HTTPGet(response, url) != 0 || response->data == NULL) {
        printf("Error fetching data for save test\n");
        Curl_Dispose(response);
        return -1;
    }
    
    // Save to file
    int save_result = meteo_SaveToFile(response->data, lat, lon);
    if (save_result == 0) {
        printf("Successfully saved weather data to file\n");
    } else {
        printf("Error saving weather data to file\n");
    }
    
    Curl_Dispose(response);
    
    return 0;
}