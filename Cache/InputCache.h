#ifndef INPUTCACHE_H
#define INPUTCACHE_H
// 

#include "../API/Meteo/Meteo.h"
#include "../API/Spotpris/Spotpris.h"

typedef struct {
    MeteoData meteoData;
    AllaSpotpriser spotprisData;
    //Userdata userdata; // Todo - Detta ska fixas
} InputCache;


int InputCache_SaveSpotpris(const AllaSpotpriser *spotpris);
int InputCache_SaveMeteo(const MeteoData *_Data);

int InputCache_PipeToAlgorithm(int fifo_fd, const InputCache *cache);

#endif