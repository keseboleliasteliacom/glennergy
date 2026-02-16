#ifndef SPOTPRIS_H
#define SPOTPRIS_H

#include <stddef.h>

// #define KVARTAR_PER_DAY 96
// #define SPOTPRIS_DAYS 3
// #define SPOTPRIS_ENTRIES (KVARTAR_PER_DAY * SPOTPRIS_DAYS)  // 288 intervals

#define SPOTPRIS_DAYS 31
#define SPOTPRIS_ENTRIES (96 * 31)  // 2976

typedef enum {
    AREA_SE1 = 0,
    AREA_SE2 = 1,
    AREA_SE3 = 2,
    AREA_SE4 = 3,
    AREA_COUNT = 4
} SpotprisArea;

typedef struct {
    char time_start[32];
    //char time_end[32];
    double sek_per_kwh;
    //double eur_per_kwh;
    //double exchange_rate;
} SpotPriceEntry;

typedef struct {
    SpotPriceEntry data[AREA_COUNT][SPOTPRIS_ENTRIES];
    size_t num_intervals[AREA_COUNT];
} AllaSpotpriser;

int spotpris_ParseJSON(SpotPriceEntry *output, int max_entries, const char *json_str);
// Fetch 3 days of spotpris for a specific area (-1, 0, +1 days)
int Spotpris_FetchArea(AllaSpotpriser *spotpris, SpotprisArea area);

// Fetch all 4 areas at once
int Spotpris_FetchAll(AllaSpotpriser *spotpris);

// Helper functions
const char* area_to_string(SpotprisArea area);
SpotprisArea string_to_area(const char *str);

#endif