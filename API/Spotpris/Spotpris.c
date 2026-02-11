#include "Spotpris.h"
#include "../../Libs/Fetcher.h"
#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../../Libs/Utils/utils.h"

// Hämta dagens datum i formatet YYYY/MM-DD (URLer måste ha YYYY/MM-DD format)
static void GetTodayDate(char *buffer, size_t size) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buffer, size, "%04d/%02d-%02d",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

// Behövs inte längre då vi inte längre vill spara till fil i den här modulen
/*
// För fil: YYYY-MM-DD (Filnamn får inte ha snestreck "/")
static void GetTodayDateFile(char *buffer, size_t size) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buffer, size, "%04d-%02d-%02d",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}
             */

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

















int Spotpris_FetchAll(AllaSpotpriser *_AllaSpotpriser)
{
    if (!_AllaSpotpriser)
    {
        return -1;
    }

    char date_str[16];
    GetTodayDate(date_str, sizeof(date_str));

    char url[256];
    printf("1\n");
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
        
        printf("2\n");
        
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
        
        printf("3\n");
        
        size_t n = json_array_size(root);
        
        strncpy(_AllaSpotpriser->areas[i].areaname, areas[i], sizeof(_AllaSpotpriser->areas[i].areaname) -1);
        _AllaSpotpriser->areas[i].areaname[sizeof(_AllaSpotpriser->areas[i].areaname) -1] = '\0';
        _AllaSpotpriser->areas[i].count = n;
        
        
        printf("3.1\n");
        // Tror vi lägger till råa datan här innan vi hanterar och lägger till parsade datan?
        strncpy(_AllaSpotpriser->areas[i].raw_json_data, resp.data, sizeof(_AllaSpotpriser->areas[i].raw_json_data) -1);
        _AllaSpotpriser->areas[i].raw_json_data[sizeof(_AllaSpotpriser->areas[i].raw_json_data) -1] = '\0';
        

        printf("4\n");
        for (size_t j = 0; j < n && j < 96; j++) // Gå på 96/kvartar istället? 96 är magic numer, kanske definera? Vi vet dock att spotpris aldrig kommer ge mer än 96 kvartar, så maybe its fine.
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

/* Gammal version
int Spotpris_Fetch(DagligSpotpris *spotpris, const char *area)
{
    if (!spotpris || !area) return -1;

    char date_str[16];
    GetTodayDate(date_str, sizeof(date_str));

    char url[256];
    snprintf(url, sizeof(url),
             "https://www.elprisetjustnu.se/api/v1/prices/%s_%s.json",
             date_str, area);

    CurlResponse resp;
    Curl_Initialize(&resp);

    int rc = Curl_HTTPGet(&resp, url);
    if (rc != 0) {
        Curl_Dispose(&resp);
        return rc;
    }

    // Parse JSON
    json_error_t error;
    json_t *root = json_loads(resp.data, 0, &error);
    Curl_Dispose(&resp);
    if (!root) {
        fprintf(stderr, "JSON parse error: %s\n", error.text);
        return -2;
    }

    if (!json_is_array(root)) {
        json_decref(root);
        return -3;
    }

    size_t n = json_array_size(root);
    /*spotpris->entries = malloc(sizeof(SpotPriceEntry) * n);
    if (!spotpris->entries) {
        json_decref(root);
        return -4;
    } 

    // GLöm inte fixa nullterminering
    strncpy(spotpris->area, area, sizeof(spotpris->area)-1);
    spotpris->area[sizeof(spotpris->area)-1] = '\0';
    spotpris->count = n;

    for (size_t i = 0; i < n; i++) {
        json_t *obj = json_array_get(root, i);

        const char *start = json_string_value(json_object_get(obj, "time_start"));
        const char *end   = json_string_value(json_object_get(obj, "time_end"));
        spotpris->entries[i].sek_per_kwh = json_real_value(json_object_get(obj, "SEK_per_kWh"));
        spotpris->entries[i].eur_per_kwh = json_real_value(json_object_get(obj, "EUR_per_kWh"));
        spotpris->entries[i].exchange_rate = json_real_value(json_object_get(obj, "EXR"));

        // Glöm inte nullterminatorn här också
        strncpy(spotpris->entries[i].time_start, start, sizeof(spotpris->entries[i].time_start));
        spotpris->entries[i].time_start[sizeof(spotpris->entries[i].time_start)-1] = '\0';

        strncpy(spotpris->entries[i].time_end, end, sizeof(spotpris->entries[i].time_end));
        spotpris->entries[i].time_end[sizeof(spotpris->entries[i].time_end)-1] = '\0';
    }

    json_decref(root);
    return 0;
}
    */



/*
// Exempel på sparad fil: spotpris_SE1_2026-01-26.json
int Spotpris_SaveToFile(const DagligSpotpris *spotpris)
{
    if (!spotpris || !spotpris->entries) return -1;

    // Finns cache-foldern?
    const char *cache_folder = "cache_spotpris";
    dir_result_t dir_res = create_folder(cache_folder);
    if (dir_res == DIR_ERROR) {
        fprintf(stderr, "Error creating cache folder: %s\n", cache_folder);
        return -2;
    }

    char filename[64];
    char date_str[16];
    GetTodayDateFile(date_str, sizeof(date_str));
    snprintf(filename, sizeof(filename), "%s/spotpris_%s_%s.json", cache_folder, spotpris->area, date_str);

    json_t *root = json_array();
    for (size_t i = 0; i < spotpris->count; i++) {
        json_t *obj = json_object();
        json_object_set_new(obj, "time_start", json_string(spotpris->entries[i].time_start));
        json_object_set_new(obj, "time_end",   json_string(spotpris->entries[i].time_end));
        json_object_set_new(obj, "SEK_per_kWh", json_real(spotpris->entries[i].sek_per_kwh));
        json_object_set_new(obj, "EUR_per_kWh", json_real(spotpris->entries[i].eur_per_kwh));
        json_object_set_new(obj, "EXR",        json_real(spotpris->entries[i].exchange_rate));
        json_array_append_new(root, obj);
    }
    
    printf("Saving %zu entries to file: spotpris_%s_%s.json\n", spotpris->count, spotpris->area, date_str);


    int ret = json_dump_file(root, filename, JSON_INDENT(4));
    json_decref(root);
    return ret;
}
*/
