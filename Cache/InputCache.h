#ifndef INPUTCACHE_H
#define INPUTCACHE_H

#include "../Libs/Utils/types.h"

int inputcache_Init(InputCache_t *cache, const char* file_path); //can do more?
int inputcache_CreateSocket(void);
int inputcache_OpenFIFOs(int *meteo_fd, int *spotpris_fd);

void inputcache_HandleRequest(InputCache_t *cache, int client_fd);
void inputcache_HandleMeteoData(InputCache_t *cache, int meteo_fd);
void inputcache_HandleSpotprisData(InputCache_t *cache, int spotpris_fd);

void inputcache_Cleanup(InputCache_t *cache); //also can do more?
#endif
