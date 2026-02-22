#ifndef AVERAGE_H
#define AVERAGE_H

#include <stddef.h>

#include "../../Cache/InputCache.h"
#include "testreader.h"


typedef struct {
    double min;
    double max;
    double average;
    double median;
    double q25;
    double q75;
} Stats_t;

typedef struct {
    Stats_t temperature;
    Stats_t ghi;
}   MeteoStats_t;

typedef struct {
    Stats_t area[4];
} SpotStats_t;

// typedef struct {

//     MeteoStats_t *meteo;
//     size_t meteo_valcount;

//     SpotStats_t *spotpris;
//     size_t spotpris_valcount;
// } AlgoInfluencer_t;

// void algoinfluencer_Init(AlgoInfluencer_t *influencer);
// void algoinfluencer_Cleanup(AlgoInfluencer_t *influencer);

// char* read_FileInMemory(const char *filepath, size_t *file_size);

// //int algoinfluencer_LoadHomesystem(AlgoInfluencer_t *influencer, const char *home_filepath);
//int algoinfluencer_LoadMeteo(AlgoInfluencer_t *influencer);
int average_SpotprisStats(SpotStats_t *spot, InputCache_t *cache);


#endif