#ifndef HOMESYSTEM_H
#define HOMESYSTEM_H


#define NAME_MAX 128
#define AREA_MAX 4

typedef struct {
    char home_id[NAME_MAX];
    double panel_capacitykw;      // Total installed solar capacity (kW)
    double panel_tiltdegrees;            // Panel tilt angle (0-90 degrees)
    double panel_azimuthdegrees;         // Panel orientation (0=North, 90=East, 180=South, 270=Wedouble
    
    double lat;
    double lon;
    //GEO location city;                        // User's geographic location
    char electricity_area[AREA_MAX];            // Swedish price area: "SE1", "SE2", "SE3", "SE4"
} Homesystem_t;

Homesystem_t* homesystem_Create(Homesystem_t *hs);
void homesystem_Destroy(Homesystem_t **hs);

int homesystem_LoadAll(Homesystem_t **systems, const char* config_path);  // Load all systems from config
int homesystem_Append(const char *json_data, const char* config_path);  // Append new system
void homesystem_update(const char *json_data, int home_id, const char* config_path);  // Update existing system



#endif // HOMESYSTEM_H
