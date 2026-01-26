#ifndef SPOTPRIS_H
#define SPOTPRIS_H

#include <stddef.h>

// TIme eller inte?

typedef struct {
    char time_start[32];
    char time_end[32];
    double sek_per_kwh;
    double eur_per_kwh;
    double exchange_rate;
} SpotPriceEntry;


typedef struct
{
    char* area[4]; // Prisklasser/områden: "SE1", "SE2", "SE3", "SE4"
    size_t count; // Kommer troligtvis vara 96 kvartar
    SpotPriceEntry *entries;
} DagligSpotpris;

// Hämta data för en prisklass och dagens datum
// Ska vi hämta alla områden på en gång istället, eller ett call för varje area?
int Spotpris_Fetch(DagligSpotpris *spotpris, const char *area);

// Spara DagligSpotpris till fil (pretty-print)
int Spotpris_SaveToFile(const DagligSpotpris *spotpris);

#endif