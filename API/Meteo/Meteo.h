#ifndef METEO_H
#define METEO_H

#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>
#include "../../Libs/Fetcher.h"
#include "../../shared/types.h"
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

int Meteo_Initialize(MeteoData *_MeteoData);
int meteo_Fetch(MeteoData *_MeteoData);
int Meteo_LoadGlennergy(MeteoData *_MeteoData);
void Meteo_Dispose(MeteoData* _MeteoData);


#endif // METEO_H
