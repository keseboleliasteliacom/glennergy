#include "constants.h"

//Homesystem
typedef struct {
    int id;
    char city[NAME_MAX];
    double panel_capacitykwh;           // Total installed solar capacity (kWh)
    double panel_tiltdegrees;           // Panel tilt angle (0-90 degrees)
    double panel_azimuthdegrees;        // Panel orientation (0=North, 90=East, 180=South, 270=West)

    double lat;
    double lon;
    char electricity_area[AREA_MAX];            // Swedish price area: "SE1", "SE2", "SE3", "SE4"

} Homesystem_t;

//Spotpris
typedef struct {
    char time_start[32];
    //char time_end[32];
    double sek_per_kwh;
    //double eur_per_kwh;
    //double exchange_rate;
} SpotPriceEntry;


typedef struct
{
    char areaname[4]; // Prisklasser/områden: "SE1", "SE2", "SE3", "SE4"
    size_t count; // Kommer troligtvis vara 96 kvartar
    SpotPriceEntry kvartar[96];
    char raw_json_data[16384]; // Vid ett test av spotprisdatan var den 13600
} DagligSpotpris;

typedef struct
{
    DagligSpotpris areas[4]; // "SE1", "SE2", "SE3", "SE4"
} AllaSpotpriser;

//Meteo
typedef struct
{
    char time_start[32];
    float temp;              // Celsius
    float ghi;               // Global Horizontal Irradiance (W/m²) - calculated from DNI + diffuse
    float dni;               // Direct Normal Irradiance (W/m²)
    float diffuse_radiation; // Diffuse Radiation (W/m²)
    float cloud_cover;       // Cloud cover percentage (0-100%)
    int is_day;
    bool valid;
} Samples;

typedef struct
{
    int id;
    char property_name[NAME_MAX];
    double lat;
    double lon;
    Samples sample[KVARTAR_TOTALT];
    char raw_json_data[RAW_DATA_MAX];
} PropertyInfo;

typedef struct
{
    PropertyInfo pInfo[PROPERTIES_MAX];
    size_t pCount;
} MeteoData;

//InputCache
typedef enum {
    AREA_SE1 = 0,
    AREA_SE2 = 1,
    AREA_SE3 = 2,
    AREA_SE4 = 3,
    AREA_COUNT = 4
} SpotprisArea;

typedef struct {
    int id;
    char city[NAME_MAX];
    double lat;
    double lon;
    Samples sample[KVARTAR_TOTALT];
} Meteo_t;

typedef struct {
    char time_start[32];
    double sek_per_kwh;
} SpotEntry_t;

typedef struct {
    SpotEntry_t data[AREA_COUNT][96];
    size_t count[AREA_COUNT];
} Spot_t;

typedef struct {

    Homesystem_t home[MAX];
    size_t home_count;

    Meteo_t meteo[MAX];
    size_t meteo_count;

    Spot_t spotpris; // Spot_t spotpris[AREA_MAX][SAMPLES]
} InputCache_t;
