#ifndef HOMESYSTEM_H
#define HOMESYSTEM_H

#include <stddef.h>
#include "Utils/types.h"

Homesystem_t* homesystem_Create(Homesystem_t *hs);
void homesystem_Destroy(Homesystem_t **hs);

int homesystem_LoadAll(Homesystem_t **systems, const char* file_path);

int homesystem_LoadAllCount(Homesystem_t *systems, const char* file_path, size_t max_count);

int homesystem_Append(const char *json_data, const char* config_path);
void homesystem_update(const char *json_data, int home_id, const char* config_path);



#endif // HOMESYSTEM_H
