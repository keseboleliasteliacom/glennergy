#ifndef FUSION_H
#define FUSION_H

#include <stddef.h>
#include <time.h>
#include <stdbool.h>
#include "../xoxoldAPI/homesystem/homesystem.h"


#define FUSION_MAX_ENTRIES 288 + 4  // 3 days * 96 + leap

typedef struct {
    char timestamp[32];
    double temperature;
    double ghi;
    //double wind_speed;
    double sek_per_kwh;
    bool valid;
}FusedDataEntry_t;

typedef struct {
    FusedDataEntry_t *comb_data[FUSION_MAX_ENTRIES];
    size_t count;

    Homesystem_t *home; 

    time_t start_timeinterval;
    time_t end_timeinterval;
    char meteo_source[256];     // cache file //source: lat_lon_YYYYMMDD.json?
    char spotpris_source[256];  // cache file //source: AREA_YYYYMMDD.json?
}FusedDataSet_t;

void fusion_Init(FusedDataSet_t *dataset);
int fusion_LoadData(FusedDataSet_t *dataset, const char *home_id, time_t date);
int fusion_MergeData(FusedDataSet_t *dataset, Meteodata *meteo)
FusedDataEntry_t* fusion_FindTimestamp(FusedDataSet_t *dataset, const char *timestamp);
void fusion_Cleanup(FusedDataSet_t *dataset);


#endif // FUSION_H