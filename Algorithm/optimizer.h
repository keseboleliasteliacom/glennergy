#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "../Cache/InputCache.h"
#include "../Cache/CacheProtocol.h"
#include "average.h"

// Optimize energy usage for a single home  
int optimize_HomeEnergy(InputCache_t *cache, int home_idx, double *solar_predictions, SpotStats_t *spot_stats, AlgorithmResults_t *result);



#endif