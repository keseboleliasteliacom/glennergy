/**
 * @file cache.h
 * @brief Filesystem-based cache with TTL support.
 *
 * @details
 * This module provides a simple persistent cache using the filesystem.
 * Each cache entry is stored as a JSON file:
 *
 *   <cache_dir>/<key>.json
 *
 * Features:
 * - Time-To-Live (TTL) validation
 * - Atomic writes (via temporary file + rename)
 * - Optional thread-safety via mutex
 *
 * No internal memory caching is performed.
 */

#ifndef CACHE_H
#define CACHE_H

#include <stddef.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

/**
 * @brief Cache instance.
 */
typedef struct Cache {
    char *cache_dir;          /**< Base directory for cache files */
    time_t ttl;               /**< Time-To-Live in seconds (0 = no expiry) */
    pthread_mutex_t mutex;    /**< Mutex for thread-safety */
} Cache;

/**
 * @brief Initialize cache instance.
 *
 * @param[out] cache Cache instance
 * @param[in] cache_dir Directory path for cache storage
 * @param[in] set_ttl TTL in seconds
 *
 * @return 0 on success, -1 on failure
 *
 * @pre cache != NULL
 * @pre cache_dir != NULL
 *
 * @post cache is initialized and ready for use
 *
 * @note Directory must exist prior to initialization
 */
int cache_Init(Cache *cache, const char *cache_dir, time_t set_ttl);

/**
 * @brief Retrieve cached data.
 *
 * @param[in] cache Cache instance
 * @param[in] key Cache key
 * @param[out] data Pointer to allocated buffer (caller owns memory)
 * @param[out] size Size of returned data
 *
 * @return true if data was successfully retrieved, false otherwise
 *
 * @pre cache != NULL
 * @pre key != NULL
 * @pre data != NULL
 *
 * @post *data contains heap-allocated buffer (must be freed by caller)
 *
 * @note Returns false if file does not exist or is expired
 */
bool cache_Get(Cache *cache, const char *key, char **data, size_t *size);

/**
 * @brief Store data in cache.
 *
 * @param[in] cache Cache instance
 * @param[in] key Cache key
 * @param[in] data Data buffer
 * @param[in] size Size of data in bytes
 *
 * @return 0 on success, -1 on failure
 *
 * @pre cache != NULL
 * @pre key != NULL
 * @pre data != NULL
 *
 * @note Uses atomic write (temp file + rename)
 */
int cache_Set(Cache *cache, const char *key, const char *data, size_t size);

/**
 * @brief Check if cache entry exists and is valid.
 *
 * @param[in] cache Cache instance
 * @param[in] key Cache key
 *
 * @return true if valid, false otherwise
 *
 * @pre cache != NULL
 * @pre key != NULL
 *
 * @note TTL is enforced if ttl > 0
 */
bool cache_IsValid(Cache *cache, const char *key);

/**
 * @brief Cleanup cache resources.
 *
 * @param[in,out] cache Cache instance
 *
 * @post cache is reset and mutex destroyed
 */
void cache_Dispose(Cache *cache);

#endif // CACHE_H