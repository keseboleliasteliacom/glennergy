#ifndef INPUTCACHE_H
#define INPUTCACHE_H
// 

#include "../API/Meteo/Meteo.h"
#include "../API/Spotpris/Spotpris.h"

typedef struct {
    MeteoData meteoData;
    DagligSpotpris spotprisData;
    //Userdata userdata; // Todo - Detta ska fixas
} InputCache;


int InputCache_SaveSpotpris(const AllaSpotpriser *spotpris);
int InputCache_SaveMeteo(const MeteoData *_Data);

#endif