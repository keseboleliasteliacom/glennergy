#define MODULE_NAME "MAIN"
#include "../Server/Log/Logger.h"
#include "InputCache.h"
#include "../Libs/Pipes.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/select.h>
#include "../Server/SignalHandler.h"

int main()
{
    int ret = -1;
    log_Init("cache.log");
    SignalHandler_Initialize();
    LOG_INFO("Starting Cache module...");

    InputCache_t *cache = NULL;
    int meteo_fd = -1, spotpris_fd = -1, socket_fd = -1;

    cache = malloc(sizeof(InputCache_t));
    if (!cache) {
        LOG_ERROR("malloc() Failed to allocate memory for InputCache");
        goto cleanup;
    }
    memset(cache, 0, sizeof(InputCache_t));

    if (inputcache_Init(cache, "/etc/Glennergy-Fastigheter.json") != 0) {
        LOG_ERROR("Failed to initialize InputCache");
        goto cleanup;
    }

    if (inputcache_OpenFIFOs(&meteo_fd, &spotpris_fd) != 0) {
        LOG_ERROR("Failed to open FIFOs");
        goto cleanup;
    }

    socket_fd = inputcache_CreateSocket();
    if (socket_fd < 0) {
        LOG_ERROR("Failed to create socket");
        goto cleanup;
    }

    LOG_INFO("InputCache ready - entering event loop...");

    while(!SignalHandler_Stop())
    {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(meteo_fd, &read_fds);
        FD_SET(spotpris_fd, &read_fds);
        FD_SET(socket_fd, &read_fds);

        int max_fd = meteo_fd;
        if (spotpris_fd > max_fd) max_fd = spotpris_fd;
        if (socket_fd > max_fd) max_fd = socket_fd;

        int ready = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (ready < 0) {
            if (errno == EINTR)
                continue;
            LOG_ERROR("select() error: %s", strerror(errno));
            break;
        }
        if (FD_ISSET(meteo_fd, &read_fds)) {
            inputcache_HandleMeteoData(cache, meteo_fd);
        }
        if (FD_ISSET(spotpris_fd, &read_fds)) {
            inputcache_HandleSpotprisData(cache, spotpris_fd);
        }
        if (FD_ISSET(socket_fd, &read_fds)) {
            int client_fd = accept(socket_fd, NULL, NULL);
            if (client_fd < 0) {
                LOG_ERROR("accept() failed: %s", strerror(errno));
            } else {
                inputcache_HandleRequest(cache, client_fd);
            }
        }
    }

    ret = 0; // Success

cleanup:
    LOG_INFO("Shutting down Cache module...");
    
    if (meteo_fd >= 0)
        close(meteo_fd);
    if (spotpris_fd >= 0)
        close(spotpris_fd);

    if (socket_fd >= 0) {
        close(socket_fd);
        unlink(CACHE_SOCKET_PATH);
    }

    free(cache);
    log_Cleanup();
    return ret;
}