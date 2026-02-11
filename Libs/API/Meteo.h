#ifndef METEO_H
#define METEO_H

#include <stdbool.h>
#include <stddef.h>
/**
 * Fetch meteo forecast from Open-Meteo API
 * 
 * @param lat Latitude
 * @param lon Longitude
 * @param meteo_out Output array for hourly meteo data
 * @param max_hours Maximum hours to fetch (up to 72)
 * @return Number of hours fetched, or -1 on error
 */
// meteo data from API (hourly)
typedef struct {
    char time[32];
    float temp;                  // Celsius
    float ghi;                   // Global Horizontal Irradiance (W/m²) - calculated from DNI + diffuse
    float dni;                   // Direct Normal Irradiance (W/m²)
    float diffuse_radiation;     // Diffuse Radiation (W/m²)
    float cloud_cover;           // Cloud cover percentage (0-100%)
    int is_day;
    bool valid;
} MeteoData;

int meteo_Fetch(double lat, double lon, MeteoData *meteo_out, int max_hours);
int meteo_SaveToFile(const char *data, double lat, double lon);

#endif // METEO_H
