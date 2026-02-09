#include "Spotpris.h"
#include "../../Libs/Fetcher.h"
#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../../Libs/Utils/utils.h"


// Support funktions för internt användning
// Hämta dagens datum i formatet YYYY/MM-DD (URLer måste ha YYYY/MM-DD format)
static void GetTodayDate(char *buffer, size_t size) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buffer, size, "%04d/%02d-%02d",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

// Print funktions för debug, kan användas av andra moduler
// SpotPriceEntry
void SpotPriceEntry_Print(const SpotPriceEntry *e)
{
    if (!e) return;
    printf("  [%s - %s]\n", e->time_start, e->time_end);
    printf("    SEK: %.5f, EUR: %.5f, EXR: %.5f\n",
           e->sek_per_kwh, e->eur_per_kwh, e->exchange_rate);
}

// DagligSpotpris
void DagligSpotpris_Print(const DagligSpotpris *d)
{
    if (!d) return;
    printf("Area: %s, count: %zu\n", d->areaname, d->count);
    for (size_t i = 0; i < d->count && i < 4; i++)
        SpotPriceEntry_Print(&d->kvartar[i]);
    printf("Raw JSON (first 20 chars): %.20s\n", d->raw_json_data);
}

// AllaSpotpriser
void AllaSpotpriser_Print(const AllaSpotpriser *a)
{
    if (!a) return;
    for (int i = 0; i < 4; i++)
        DagligSpotpris_Print(&a->areas[i]);
}



// Levererar en populera AllaSpotpriser struct
int Spotpris_FetchAll(AllaSpotpriser *_AllaSpotpriser)
{
    if (!_AllaSpotpriser)
    {
        return -1;
    }

    char date_str[16];
    GetTodayDate(date_str, sizeof(date_str));

    char url[256];
    const char *areas[4] = {"SE1", "SE2", "SE3", "SE4"};

    for (int i = 0; i < 4; i++)
    {
        snprintf(url, sizeof(url),
        "https://www.elprisetjustnu.se/api/v1/prices/%s_%s.json",
        date_str, areas[i]);

        CurlResponse resp;
        Curl_Initialize(&resp); 
        
        int rc = Curl_HTTPGet(&resp, url);
        if (rc != 0)
        {
            Curl_Dispose(&resp);
            return rc;
        }
                
        // Parsa JSON så vi kan skicka med det
        json_error_t error;
        json_t *root = json_loads(resp.data, 0, &error);
        
        if (!root)
        {
            fprintf(stderr, "JSON parse error: %s\n", error.text);
            Curl_Dispose(&resp);
            return -2;
        }
        
        if (!json_is_array(root))
        {
            json_decref(root);
            return -3;
        }
                
        size_t n = json_array_size(root);
        
        strncpy(_AllaSpotpriser->areas[i].areaname, areas[i], sizeof(_AllaSpotpriser->areas[i].areaname) -1);
        _AllaSpotpriser->areas[i].areaname[sizeof(_AllaSpotpriser->areas[i].areaname) -1] = '\0';
        _AllaSpotpriser->areas[i].count = n;
        
        // Lägg till råa JSON-datan så InputCache kan spara ner det. Glömm inte nullterminering
        strncpy(_AllaSpotpriser->areas[i].raw_json_data, resp.data, sizeof(_AllaSpotpriser->areas[i].raw_json_data) -1);
        _AllaSpotpriser->areas[i].raw_json_data[sizeof(_AllaSpotpriser->areas[i].raw_json_data) -1] = '\0';
        
        // Todo - Kanske onödigt med hårdkodat att vi aldrig läser mer än 96 kvartar, eftersom SpotPris bara kommer leverera 96?
        for (size_t j = 0; j < n && j < 96; j++) 
        {
            json_t *obj = json_array_get(root, j);

            const char *start = json_string_value(json_object_get(obj, "time_start"));
            const char *end   = json_string_value(json_object_get(obj, "time_end"));
            _AllaSpotpriser->areas[i].kvartar[j].sek_per_kwh = json_real_value(json_object_get(obj, "SEK_per_kWh"));
            _AllaSpotpriser->areas[i].kvartar[j].eur_per_kwh = json_real_value(json_object_get(obj, "EUR_per_kWh"));
            _AllaSpotpriser->areas[i].kvartar[j].exchange_rate = json_real_value(json_object_get(obj, "EXR"));
            
            // Null terminator här också
            strncpy(_AllaSpotpriser->areas[i].kvartar[j].time_start, start, sizeof(_AllaSpotpriser->areas[i].kvartar[j].time_start));
            _AllaSpotpriser->areas[i].kvartar[j].time_start[sizeof(_AllaSpotpriser->areas[i].kvartar[j].time_start) -1] = '\0';
            
            strncpy(_AllaSpotpriser->areas[i].kvartar[j].time_end, end, sizeof(_AllaSpotpriser->areas[i].kvartar[j].time_end));
            _AllaSpotpriser->areas[i].kvartar[j].time_end[sizeof(_AllaSpotpriser->areas[i].kvartar[j].time_end) -1] = '\0';
        }
        
        Curl_Dispose(&resp);
        printf("5\n");
        json_decref(root);
    }

    return 0;
}