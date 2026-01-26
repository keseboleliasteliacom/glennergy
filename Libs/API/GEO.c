#include "../Fetcher.h"
#include "GEO.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

//TODO: Save the user location (JSON), free API offers only 100 monthly

GEO* GEO_Init()
{
    GEO *coordinates = (GEO*)malloc(sizeof(GEO));
    
    if(!coordinates){
        perror("Malloc Failed");
        exit(1);
    }
    
    return coordinates;
}

GEO* GEO_Get_Coord_IP()
{
    
    GEO *coord = GEO_Init();
    
    char* url = "https://ipapi.co/json/";
    
    CurlResponse* response = (CurlResponse*)malloc(sizeof(CurlResponse));
    
    
    if(Curl_Initialize(response) < 0){
        
        exit(1);
    } 

    
    if(Curl_HTTPGet(response, url) < 0){
        
        exit(1);
    } 

    
    char* lat = strstr(response->data, "\"latitude\":");
    
    char* lon = strstr(response->data, "\"longitude\":");
    

    if(lat && lon) {
        coord->latitude = atof(lat + 11);
        coord->longitude = atof(lon + 12);
    }

    printf("Lat: %f, lon: %f\n", coord->latitude, coord->longitude);

    Curl_Dispose(response);

    return coord;
}

void GEO_Dispose(GEO* name)
{
    if(name == NULL) return;
    free(name);
    name = NULL;
}