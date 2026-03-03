#ifndef SOLAR_H
#define SOLAR_H

#include "../Cache/InputCache.h"
#include "../Libs/Homesystem.h"

int solar_PredictHome(InputCache_t *cache, int home_idx, double *solar_output);

#endif