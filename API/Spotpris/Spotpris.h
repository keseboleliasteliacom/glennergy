#ifndef SPOTPRIS_H
#define SPOTPRIS_H

#include <stddef.h>

typedef struct {
    char time_start[32];
    char time_end[32];
    double sek_per_kwh;
    double eur_per_kwh;
    double exchange_rate;
} SpotPriceEntry;


typedef struct
{
    char areaname[4]; // Prisklasser/områden: "SE1", "SE2", "SE3", "SE4"
    size_t count; // Kommer troligtvis vara 96 kvartar
    SpotPriceEntry kvartar[96];
    char raw_json_data[16384]; // Vid ett test av spotprisdatan var den 13600 
} DagligSpotpris;

typedef struct 
{
    DagligSpotpris areas[4]; // "SE1", "SE2", "SE3", "SE4"
} AllaSpotpriser;


// Print / debug functions
void SpotPriceEntry_Print(const SpotPriceEntry *e);
void DagligSpotpris_Print(const DagligSpotpris *d);
void AllaSpotpriser_Print(const AllaSpotpriser *a);



// Hämta data för en prisklass och dagens datum
// Ska vi hämta alla områden på en gång istället, eller ett call för varje area?
int Spotpris_Fetch(DagligSpotpris *spotpris, const char *area);

int Spotpris_FetchAll(AllaSpotpriser *_AllaSpotpriser);

// Spara DagligSpotpris till fil (pretty-print)
int Spotpris_SaveToFile(const DagligSpotpris *spotpris);

#endif