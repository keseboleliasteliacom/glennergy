#ifndef INPUTCACHE_H
#define INPUTCACHE_H
// 

#include "../API/Meteo.h"
#include "../API/Spotpris.h"

typedef struct {
    MeteoData meteoData;
    DagligSpotpris spotprisData;
    //Userdata userdata;
} InputCache;


#endif