// #define MODULE_NAME "ALGOINFLUENCER"
// #include "algoinfluencer.h"
// #include "../../server/log/logger.h"
 #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/stat.h>
#include "algoinfluencer.h"
#include "../../Cache/InputCache.h"

// #define MAX_FILE_SIZE (10 * 1024 * 1024) // 10 MB
// #define ESTIMATED_MAX_MALLOC 15000 //

// int algoinfluencer_CalculateSpotpris(SpotStats_t *_sek_per_kwh, InputCache_t *cache)
// {
//     if (!cache || cache->spotpris.areas[0].count == 0) {
//         fprintf(stderr, "No spotpris data available in cache\n");
//         return -1;
//     }

//     double sum = 0.0;
//     double min = cache->spotpris.areas[0].kvartar[0].sek_per_kwh;
//     double max = min;

//     size_t total_entries = 0;

//     for (size_t i = 0; i < sizeof(cache->spotpris.areas) / sizeof(cache->spotpris.areas[0]); i++) {
//         AreaSpotpris_t *area = &cache->spotpris.areas[i];
//         for (size_t j = 0; j < area->count; j++) {
//             double price = area->kvartar[j].sek_per_kwh;
//             sum += price;
//             if (price < min) min = price;
//             if (price > max) max = price;
//             total_entries++;
//         }
//     }

//     if (total_entries == 0) {
//         fprintf(stderr, "No spotpris entries found in cache\n");
//         return -1;
//     }

//     _sek_per_kwh->min = min;
//     _sek_per_kwh->max = max;
//     _sek_per_kwh->average = sum / total_entries;

//     // Median and quartiles calculation would go here (not implemented)

//     return 0;
// }

// char* read_FileInMemory(const char* filepath, size_t* max_size) //Utils?
// {
//     FILE* fileptr = fopen(filepath, "rb");
//     if (!fileptr) {
//         LOG_ERROR("fopen() failed for file: %s", filepath);
//         return NULL;
//     }

//     struct stat fileinfo;
//     if (fstat(fileno(fileptr), &fileinfo) != 0) {
//         LOG_ERROR("stat() failed for %s", filepath);
//         fclose(fileptr);
//         return NULL;
//     }
//     // if (!S_ISREG(fileinfo.st_mode)) {                      
//     //     LOG_ERROR("Not a regular file: %s", filepath);      //vi vet de e json
//     //     return NULL;                                       
//     // }

//     size_t filesize = fileinfo.st_size;
//     if (filesize > MAX_FILE_SIZE) {
//         LOG_ERROR("File too large: %s", filepath);
//         fclose(fileptr);
//         return NULL;
//     }

//     char* json_data = malloc((filesize + 1) * sizeof(char));
//     if (!json_data) {
//         LOG_ERROR("malloc() failed for file: %s", filepath);
//         fclose(fileptr);
//         return NULL;
//     }

//     size_t bytes_read = fread(json_data, sizeof(char), filesize, fileptr);
//     if (bytes_read != filesize) {
//         LOG_ERROR("fread() %zu / %zu bytes from :%s", bytes_read, filesize, filepath);
//         free(json_data);
//         fclose(fileptr);
//         return NULL;
//     }

//     json_data[filesize] = '\0';
//     fclose(fileptr);

//     if (max_size)
//         *max_size = bytes_read;
    
//     return json_data;
// }

// void algoinfluencer_Init(AlgoInfluencer_t *influencer)
// {
//     if (influencer) {
//         memset(influencer, 0, sizeof(AlgoInfluencer_t));
//     }
// }

// int algoinfluencer_LoadSpotpris(AlgoInfluencer_t *influencer, const char *spotpris_filepath)
// {
//     if (!influencer || !spotpris_filepath) {
//         LOG_ERROR("Invalid arguments to algoinfluencer_LoadSpotpris");
//         return -1;
//     }

//     if (influencer->spotpris) {
//         LOG_INFO("free() old spotpris data");
//         free(influencer->spotpris);
//         influencer->spotpris = NULL;
//         influencer->spotpris_valcount = 0;
//     }

//     char *json_string = read_FileInMemory(spotpris_filepath, NULL);
//     if (!json_string) {
//         LOG_ERROR("Failed to read spotpris file: %s", spotpris_filepath);
//         return -1;
//     }

//     SpotPriceEntry *entries = malloc(ESTIMATED_MAX_MALLOC * sizeof(SpotPriceEntry));
//     if (!entries) {
//         LOG_ERROR("malloc() failed for spotpris entries");
//         free(json_string);
//         return -1;
//     }

//     int count = spotpris_ParseJSON(entries, ESTIMATED_MAX_MALLOC, json_string);
//     free(json_string);

//     if (count < 0) {
//         LOG_ERROR("Failed to parse spotpris JSON");
//         free(entries);
//         return -1;
//     }

//     influencer->spotpris = entries;
//     influencer->spotpris_valcount = count;
    
//     LOG_INFO("Loaded %d spotpris entries from %s", count, spotpris_filepath);
//     return 0;
// }

// void algoinfluencer_Cleanup(AlgoInfluencer_t *influencer)
// {
//     if (!influencer) {
//         return;
//     }

//     if (influencer->home) {
//         free(influencer->home);
//         influencer->home = NULL;
//     }

//     if (influencer->meteo) {
//         free(influencer->meteo);
//         influencer->meteo = NULL;
//         influencer->meteo_valcount = 0;
//     }

//     if (influencer->spotpris) {
//         free(influencer->spotpris);
//         influencer->spotpris = NULL;
//         influencer->spotpris_valcount = 0;
//     }
// }