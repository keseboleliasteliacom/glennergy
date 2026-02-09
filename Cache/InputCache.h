#ifndef INPUTCACHE_H
#define INPUTCACHE_H
// 

#include "../API/Meteo/Meteo.h"
#include "../API/Spotpris/Spotpris.h"

typedef struct {
    MeteoData meteoData;
    DagligSpotpris spotprisData;
    //Userdata userdata;
} InputCache;


int InputCache_SaveSpotpris(const AllaSpotpriser *spotpris);

#endif