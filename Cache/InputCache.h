#ifndef INPUTCACHE_H
#define INPUTCACHE_H


#include "../API/Meteo/Meteo.h"
#include "../API/Spotpris/Spotpris.h"
#include "../Libs/Homesystem.h"

#define FIFO_METEO_READ "/tmp/fifo_meteo"
#define FIFO_SPOTPRIS_READ "/tmp/fifo_spotpris"
#define FIFO_ALGORITHM_WRITE "/tmp/fifo_algoritm"

#define MAX 5

typedef enum {
    AREA_SE1 = 0,
    AREA_SE2 = 1,
    AREA_SE3 = 2,
    AREA_SE4 = 3,
    AREA_COUNT = 4
} SpotprisArea;

typedef struct {
    int id;
    char city[NAME_MAX];
    double lat;
    double lon;
    char electricity_area[5];
    Samples sample[KVARTAR_TOTALT];
} Meteo_t;

typedef struct {
    char time_start[32];
    double sek_per_kwh;
} SpotEntry_t;

typedef struct {
    SpotEntry_t data[AREA_COUNT][96];
    size_t count[AREA_COUNT];
} Spot_t;

typedef struct {

    Homesystem_t home[MAX];
    size_t home_count;

    Meteo_t meteo[MAX];
    size_t meteo_count;

    Spot_t spotpris; // Spot_t spotpris[AREA_MAX][SAMPLES]
} InputCache_t;


int InputCache_SaveSpotpris(const AllaSpotpriser *spotpris);
int InputCache_SaveMeteo(const MeteoData *_Data);

int InputCache_LoadHomesystem(InputCache_t *cache, const char* file_path);
int InputCache_ReceiveMeteo(InputCache_t *cache, const char* fifo_path);
int InputCache_ReceiveSpotpris(InputCache_t *cache, const char* fifo_path);

int InputCache_PipeToAlgorithm(const InputCache_t *cache);

#endif