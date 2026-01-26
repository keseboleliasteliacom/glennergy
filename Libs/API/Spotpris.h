#ifndef SPOTPRIS_H
#define SPOTPRIS_H


typedef struct{

}Time;

typedef struct
{

    float latitude;
    float longitude;
    float panel_capacity_kw;     // Total installed capacity (kW)
    float panel_angle;           // Degrees (tilt angle)
    //float panel_azimuth;         // Degrees (0=North, 180=South)
    float performance_ratio;     // 0.0 - 1.0 (default: 0.8, accounts for all losses)

    char* area;
    float price_sek;
    Time* time;
} Spotpris;


#endif