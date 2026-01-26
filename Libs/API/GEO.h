#ifndef GEO_H
#define GEO_H

typedef struct
{
    float latitude;
    float longitude;
    //char *name;
} GEO;

GEO* GEO_Init();

GEO* GEO_Get_Coord_IP();

void GEO_Dispose(GEO* name);

#endif