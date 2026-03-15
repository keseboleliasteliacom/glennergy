#define MODULE_NAME "UTILS"
#include "../Server/Log/Logger.h"
#include "utils.h"
#include "../Libs/Sockets.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int getAreaIndex(const char *area)
{
    if (strcmp(area, "SE1") == 0) return 0;
    if (strcmp(area, "SE2") == 0) return 1;
    if (strcmp(area, "SE3") == 0) return 2;
    if (strcmp(area, "SE4") == 0) return 3;
    return -1;
}

int calculateMeteoOffset(const Meteo_t *meteo, const Spot_t *spotpris, int area_idx)
{
    if (meteo->sample[0].time_start[0] == '\0' || spotpris->count[area_idx] == 0) {
        return -1;
    }
    
    // Parse hour and minute from meteo start time: "2026-03-13T06:00"
    int meteo_hour, meteo_min;
    sscanf(meteo->sample[0].time_start, "%*d-%*d-%*dT%d:%d", &meteo_hour, &meteo_min);
    
    // Calculate offset in 15-min slots
    // Example: 06:00 = 6 hours × 4 slots/hour = 24 slots from 00:00
    int offset = (meteo_hour * 4) + (meteo_min / 15);
    
    LOG_DEBUG("Meteo starts at %s, spotpris offset = %d slots", meteo->sample[0].time_start, offset);
    
    return offset;
}

int cacheRequest(CacheCommand cmd, void *data_out, size_t expected_size)
{
    if (!data_out || expected_size == 0) {
        LOG_ERROR("Invalid parameters for cache_request");
        return -1;
    }

    int sock_fd = socket_Connect(CACHE_SOCKET_PATH);
    if (sock_fd < 0) {
        LOG_ERROR("Failed to connect to cache socket: %s", strerror(errno));
        return -1;
    }

    CacheRequest req = { .command = cmd };
    if (send(sock_fd, &req, sizeof(req), 0) != sizeof(req)) {
        LOG_ERROR("Failed to send request");
        close(sock_fd);
        return -1;
    }

    CacheResponse resp;
    ssize_t bytes_read = recv(sock_fd, &resp, sizeof(resp), 0);
    if (bytes_read != sizeof(resp)) {
        LOG_ERROR("Failed to receive response (got %zd bytes)", bytes_read);
        close(sock_fd);
        return -1;
    }

    if (resp.status != 0) {
        LOG_ERROR("Cache returned error status: %u", resp.status);
        close(sock_fd);
        return -1;
    }

    size_t total_read = 0;
    while (total_read < expected_size) {
        bytes_read = recv(sock_fd, (char *)data_out + total_read, expected_size - total_read, 0);
        
        if (bytes_read <= 0) {
            LOG_ERROR("Socket error during read (got %zu/%zu bytes)", total_read, expected_size);
            close(sock_fd);
            return -1;
        }
        total_read += bytes_read;
    }
    close(sock_fd);

    LOG_DEBUG("Successfully received %zu bytes", total_read);
    return 0;
}

int cacheSendResults(const ResultRequest *results)
{
    int sock_fd = socket_Connect(CACHE_SOCKET_PATH);
    if (sock_fd < 0) {
        LOG_ERROR("Failed to connect to cache");
        return -1;
    }

    CacheRequest req = { .command = CMD_SET_RESULT };
    send(sock_fd, &req, sizeof(req), 0);
    send(sock_fd, results, sizeof(ResultRequest), 0);

    CacheResponse resp;
    recv(sock_fd, &resp, sizeof(resp), 0);

    close(sock_fd);
    return (resp.status == 0) ? 0 : -1;
}