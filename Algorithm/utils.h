#ifndef ALGORITHM_UTILS_H
#define ALGORITHM_UTILS_H

#include "../Cache/CacheProtocol.h"

// Area helpers
int getAreaIndex(const char *area);

// Time/offset helpers
int calculateMeteoOffset(const Meteo_t *meteo, const Spot_t *spotpris, int area_idx);

// Cache communication
int cacheRequest(CacheCommand cmd, void *data_out, size_t expected_size);
int cacheSendResults(const ResultRequest *results);

#endif