#include "../../Libs/Pipes.h"
#include "Meteo.h"
#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>

int main()
{
    MeteoData data[127];
    meteo_Fetch(&data, 127);

    for(int i = 0; i < 127; i++)
    {
        printf("fdsfsd %d %.f\n", i, data[i].cloud_cover);
    }


    return 0;
}