#define _POSIX_C_SOURCE 200809L
#include "Cache.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <pthread.h>

#define MAX_CACHE_SIZE (10 * 1024 * 1024)  // 10MB limit



int cache_Init(Cache *cache, const char *cache_dir)
{
    if (!cache || !cache_dir)
        return -1;
    
    cache->cache_dir = strdup(cache_dir);
    if (!cache->cache_dir)
        return -1;

    mkdir(cache->cache_dir, 0755);
    
    if (pthread_mutex_init(&cache->mutex, NULL) != 0)
    {
        free(cache->cache_dir);
        return -1;
    }
    
    printf("[CACHE] Initialized: %s\n", cache->cache_dir);
    return 0;
}

bool cache_Get(Cache *cache, const char *key, char **buffer, size_t *size) //GetFromFile atm
{
    if (!cache || !key || !buffer || !size)
        return false;
        
    bool result = false;
    char filename[128];
    char first_line[256];
    time_t cached_time, TTL;
    char *json_line = NULL;
    size_t len = 0;
    ssize_t read_count;

    //pthread_mutex_lock(&cache->mutex);
    snprintf(filename, sizeof(filename), "%s/%s.json", cache->cache_dir, key);

    FILE *fptr = fopen(filename, "r");
    if (!fptr)
    {
        //pthread_mutex_unlock(&cache->mutex);
        return false;  // Quick exit - no cleanup needed
    }
    
    if (!fgets(first_line, sizeof(first_line), fptr))
        goto cleanup;
        
    if (sscanf(first_line, "%ld %ld", &cached_time, &TTL) != 2)
    {
        fprintf(stderr, "[CACHE] Invalid format in %s\n", filename);
        goto cleanup;
    }

    if (TTL > 0 && (time(NULL) - cached_time) > TTL)
    {
        printf("[CACHE] Expired: %s\n", key);
        goto cleanup;
    }

    read_count = getline(&json_line, &len, fptr);
    if (read_count == -1)
    {
        fprintf(stderr, "[CACHE] Failed to read data from %s\n", filename);
        goto cleanup;
    }
    if (read_count > MAX_CACHE_SIZE || read_count < 0)
    {
        free(json_line);
        fprintf(stderr, "[CACHE] Data too large in %s\n", filename);
        goto cleanup;
    }

    *buffer = json_line;  // Caller must free()
    *size = read_count;
    result = true;

cleanup:
    fclose(fptr);
    //pthread_mutex_unlock(&cache->mutex);
    if(result)
        printf("[CACHE] Hit: %s (size: %zd)\n", key, read_count);
    return result;
}

int cache_Set(Cache *cache, const char *key, const char *data, size_t size, time_t TTL)     //SaveToFile atm
{
    if (!cache || !key || !data)
        return -1;

    char filename[128];
    //pthread_mutex_lock(&cache->mutex);
    snprintf(filename, sizeof(filename), "%s/%s.json", cache->cache_dir, key);

    FILE *fptr = fopen(filename, "w");
    if (!fptr)
    {
        fprintf(stderr, "[CACHE] Failed to open %s for writing\n", filename);
        //pthread_mutex_unlock(&cache->mutex);
        return -1;
    }

    fprintf(fptr, "%ld %ld\n", time(NULL), TTL);
    fputs(data, fptr);
    fclose(fptr);
    
    //pthread_mutex_unlock(&cache->mutex);
    printf("[CACHE] Saved: %s/%s.json (TTL: %ld)\n", cache->cache_dir, key, TTL);
    return 0;
}

void cache_Dispose(Cache *cache)
{
    if (!cache)
        return;
    
    free(cache->cache_dir);
    pthread_mutex_destroy(&cache->mutex);
    
    printf("[CACHE] Disposed\n");
}