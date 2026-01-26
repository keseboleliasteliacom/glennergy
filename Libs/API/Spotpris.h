#ifndef SPOTPRIS_H
#define SPOTPRIS_H

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


#endif