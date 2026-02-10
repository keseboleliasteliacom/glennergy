#ifndef HOMESYSTEM_H
#define HOMESYSTEM_H


typedef struct {
    char home_id[64];
    float solar_panel_capacity_kw;      // Total installed solar capacity (kW)
    float panel_tilt_degrees;            // Panel tilt angle (0-90 degrees)
    float panel_azimuth_degrees;         // Panel orientation (0=North, 90=East, 180=South, 270=West)
    
    float lat;
    float lon;
    //GEO location city;                        // User's geographic location
    char electricity_area[4];            // Swedish price area: "SE1", "SE2", "SE3", "SE4"
} Homesystem;

Homesystem* homesystem_Create();
void homesystem_Destroy(Homesystem **hs);

void homesystem_Load(Homesystem *hs, const char* config_path); //todo: load from file, from db in future?

int homesystem_Save(const char *request_body, const char* config_path);  // Takes JSON directly
void homesystem_update(Homesystem *request_body, const char* config_path);



#endif // HOMESYSTEM_H
