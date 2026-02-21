#ifndef INPUTCACHE_H
#define INPUTCACHE_H


#include "../API/Meteo/Meteo.h"
#include "../API/Spotpris/Spotpris.h"
#include "../Libs/Homesystem.h"

#define FIFO_METEO_READ "/tmp/fifo_meteo"
#define FIFO_SPOTPRIS_READ "/tmp/fifo_spotpris"
#define FIFO_ALGORITHM_WRITE "/tmp/fifo_algoritm"

#define MAX 5

typedef struct {
    int id;
    char city[NAME_MAX];
    double lat;
    double lon;
    Samples sample[KVARTAR_TOTALT];
} Meteo_t;

typedef struct {
    char time_start[32];
    double sek_per_kwh;
} SpotPriceEntry_t;


typedef struct {
    char areaname[4];
    size_t count;
    SpotPriceEntry_t kvartar[96]; //100
} AreaSpotpris_t;

typedef struct {
    AreaSpotpris_t areas[4]; // "SE1", "SE2", "SE3", "SE4"
} SpotprisData_t;

typedef struct {

    Homesystem_t home[MAX];
    size_t home_count;

    Meteo_t meteo[MAX];
    size_t meteo_count;

    SpotprisData_t spotpris;
} InputCache_t;


int InputCache_SaveSpotpris(const AllaSpotpriser *spotpris);
int InputCache_SaveMeteo(const MeteoData *_Data);

int InputCache_LoadHomesystem(InputCache_t *cache, const char* file_path);
int InputCache_ReceiveMeteo(InputCache_t *cache, const char* fifo_path);
int InputCache_ReceiveSpotpris(InputCache_t *cache, const char* fifo_path);

int InputCache_PipeToAlgorithm(const InputCache_t *cache);

#endif