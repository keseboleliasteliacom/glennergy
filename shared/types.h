#ifndef GLENNERGY_TYPES_H
#define GLENNERGY_TYPES_H

#include "constants.h"

//From Meteo
typedef struct
{
    char time_start[32];
    float temp;              // Celsius
    float ghi;               // Global Horizontal Irradiance (W/m²) - calculated from DNI + diffuse
    float dni;               // Direct Normal Irradiance (W/m²)
    float diffuse_radiation; // Diffuse Radiation (W/m²)
    float cloud_cover;       // Cloud cover percentage (0-100%)
    int is_day;
    bool valid;
} Samples;

typedef struct
{
    int id;
    char property_name[NAME_MAX];
    double lat;
    double lon;
    Samples sample[KVARTAR_TOTALT];
    char raw_json_data[RAW_DATA_MAX];
    char electricity_area[5];
} PropertyInfo;

typedef struct
{
    PropertyInfo pInfo[PROPERTIES_MAX];
    size_t pCount;
} MeteoData;


//From Spotpris
typedef struct {
    char time_start[32];
    //char time_end[32];
    double sek_per_kwh;
    //double eur_per_kwh;
    //double exchange_rate;
} SpotPriceEntry;


typedef struct
{
    char areaname[4]; // Prisklasser/områden: "SE1", "SE2", "SE3", "SE4"
    size_t count; // Kommer troligtvis vara 96 kvartar
    SpotPriceEntry kvartar[192];
    char raw_json_data[32000]; // Vid ett test av spotprisdatan var den 13600
} DagligSpotpris;

typedef struct
{
    DagligSpotpris areas[4]; // "SE1", "SE2", "SE3", "SE4"
} AllaSpotpriser;
#endif
