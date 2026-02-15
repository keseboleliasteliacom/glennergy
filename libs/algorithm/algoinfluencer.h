#ifndef ALGOINFLUENCER_H
#define ALGOINFLUENCER_H

typedef struct {
    char time [32];
    float temp;
    float ghi;

} MeteofileData_t;

typedef struct {
    char time_start[32];
    double sek_per_kwh;
} SpotprisfileData_t;

typedef struct {
    Homesystem_t *home;
    MeteofileData_t *meteo;
    SpotprisfileData_t *spotpris;
} AlgoInfluencer_t;

char* read_FileInMemory(const char *filepath, size_t *file_size);

int algoinfluencer_LoadStructs(AlgoInfluencer_t *influencer, const char *home_filepath, const char *meteo_filepath, const char *spotpris_filepath);


#endif