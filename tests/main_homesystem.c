#include "../libs/API/homesystem/homesystem.h"
#include <stdlib.h>
#include <stdio.h>

int main () {


    Homesystem *systems = NULL;

    int n_systems = homesystem_LoadAll(&systems, "homesystem_config.json");

    if (n_systems > 0)
    {
        for (int i = 0; i < n_systems; i++)
        {
            //printf("Area: %s\n", systems[i].electricity_area);
            printf("Homesystem %d: ID=%s, Capacity=%.2f kW, Tilt=%.1f°, Azimuth=%.1f°, Lat=%.3f, Lon=%.3f, Area=%s\n",
                   i+1,
                   systems[i].home_id,
                   systems[i].panel_capacitykw,
                   systems[i].panel_tiltdegrees,
                   systems[i].panel_azimuthdegrees,
                   systems[i].lat,
                   systems[i].lon,
                   systems[i].electricity_area);
        }
        free (systems[0].home_id); // Free individual strings if they were dynamically allocated
        
        free(systems);
    }
// // 2. Use homesystem to determine which data files to load
// char meteo_file[256];
// snprintf(meteo_file, sizeof(meteo_file), 
//          "historicMeteo/%.2f_%.2f_%s_%s.json",
//          hs.lat, hs.lon, start_date, end_date);

// char spotpris_file[256];
// snprintf(spotpris_file, sizeof(spotpris_file),
//          "historicspotpris/spotpris_history_%s_%s_%s.json",
//          start_date, end_date, hs.electricity_area);

// // 3. Load data
// MeteoData meteo[3000];
// int meteo_count = load_meteo(meteo_file, meteo);

// SpotprisData spotpris[3000];
// int spotpris_count = load_spotpris(spotpris_file, spotpris);

// // 4. Calculate using homesystem parameters
// for (int i = 0; i < meteo_count && i < spotpris_count; i++) {
//     // Use homesystem params in calculation
//     double solar_kwh = calculate_solar(
//         meteo[i].shortwave_radiation,
//         hs.panel_capacitykw,        // From homesystem
//         hs.panel_tiltdegrees,       // From homesystem
//         hs.panel_azimuthdegrees,    // From homesystem
//         meteo[i].temperature
//     );
    
//     double value = solar_kwh * spotpris[i].sek_per_kwh;
    
//     printf("%s: %.2f kWh * %.2f SEK = %.2f SEK\n",
//            meteo[i].time, solar_kwh, spotpris[i].sek_per_kwh, value);
// }

    return 0;
}