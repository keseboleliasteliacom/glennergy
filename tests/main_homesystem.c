#define MODULE_NAME "TEST_FILE"
#include "../xoxoldAPI/homesystem/homesystem.h"
#include "../server/log/logger.h"
#include <stdlib.h>
#include <stdio.h>


//gcc -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200112L -DDEBUG -IxoxoldAPI -IxoxoldAPI/homesystem -Iserver/log tests/main_homesystem.c xoxoldAPI/homesystem/homesystem.c server/log/logger.c -ljansson -lpthread -o tests/main_homesystem

//project root then ./tests/main_homesystem

int main () {
    
    log_Init(NULL); 
    log_SetLevel(LOG_LEVEL_DEBUG);


    Homesystem_t *systems = NULL;

    int n_systems = homesystem_LoadAll(&systems, "xoldAPI/homesystem/homesystem_config.json");

    if (n_systems > 0)
    {
        for (int i = 0; i < n_systems; i++)
        {
            //printf("Area: %s\n", systems[i].electricity_area);
            printf("Homesystem %d: ID=%s, Capacity=%.2f kWh, Tilt=%.1f°, Azimuth=%.1f°, Lat=%.3f, Lon=%.3f, Area=%s\n",
                   i+1,
                   systems[i].home_id,
                   systems[i].panel_capacitykwh,
                   systems[i].panel_tiltdegrees,
                   systems[i].panel_azimuthdegrees,
                   systems[i].lat,
                   systems[i].lon,
                   systems[i].electricity_area);
        }
        
    }
    LOG_DEBUG("debug message: %s", systems[0].home_id);
    LOG_INFO("info message: %s", systems[0].electricity_area);
    LOG_WARNING("warning message: %.2f lat", systems[0].lat);
    LOG_ERROR("error message: %.2f lon", systems[0].lon);
    
    free(systems);
    log_Cleanup();

    return 0;
}