#ifndef CACHE_PROTOCOL_H
#define CACHE_PROTOCOL_H

#include <stdint.h>

#define CACHE_SOCKET_PATH "/tmp/glennergy_cache.sock"
/**
 * @file CacheProtocol.h
 * @brief IPC protocol definitions for cache communication.
 *
 * @details
 * Defines request/response structures used over UNIX socket
 * between clients and the cache service.
 */
typedef enum {
    CMD_GET_ALL = 1,
    CMD_GET_METEO = 2,
    CMD_GET_SPOTPRIS = 3,
    CMD_PING = 99
} CacheCommand;

/**
 * @brief Cache request message.
 */
typedef struct {
    uint8_t command;     /**< Command type */
    uint8_t reserved[3]; /**< Padding / future use */
} CacheRequest;

/**
 * @brief Cache response header.
 */
typedef struct {
    uint32_t status;     /**< 0 = success, 1 = error */
    uint32_t data_size;  /**< Payload size in bytes */
} CacheResponse;
#endif