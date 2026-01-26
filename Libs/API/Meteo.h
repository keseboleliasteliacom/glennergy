#ifndef METEO_H
#define METEO_H

#include <stdbool.h>

/**
 * Fetch weather forecast from Open-Meteo API
 * 
 * @param lat Latitude
 * @param lon Longitude
 * @param weather_out Output array for hourly weather data
 * @param max_hours Maximum hours to fetch (up to 72)
 * @return Number of hours fetched, or -1 on error
 */
// Weather data from API (hourly)
typedef struct {
    float temp;                  // Celsius
    float ghi;                   // Global Horizontal Irradiance (W/m²) - calculated from DNI + diffuse
    float dni;                   // Direct Normal Irradiance (W/m²)
    float diffuse_radiation;     // Diffuse Radiation (W/m²)
    float cloud_cover;           // Cloud cover percentage (0-100%)
    int is_day;
    bool valid;
} WeatherData;

int meteo_Fetch(double lat, double lon, WeatherData *weather_out, int max_hours);

#endif // METEO_H
