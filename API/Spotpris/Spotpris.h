#ifndef SPOTPRIS_H
#define SPOTPRIS_H

#include <stddef.h>

// Print / debug functions
void SpotPriceEntry_Print(const SpotPriceEntry *e);
void DagligSpotpris_Print(const DagligSpotpris *d);
void AllaSpotpriser_Print(const AllaSpotpriser *a);

int Spotpris_FetchAll(AllaSpotpriser *_AllaSpotpriser);

#endif
