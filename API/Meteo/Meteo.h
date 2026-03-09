#ifndef METEO_H
#define METEO_H

#include "../../Libs/Utils/types.h"
#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>
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
int Meteo_Parse(MeteoData *_MeteoData, const char *_JsonRaw);
void Meteo_Dispose(MeteoData* _MeteoData);


#endif // METEO_H
