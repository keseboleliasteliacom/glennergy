#include "spotpris.h"
#include "../fetcher.h"
#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Hämta dagens datum i formatet YYYY/MM-DD (URLer måste ha YYYY/MM-DD format)
static void GetTodayDate(char *buffer, size_t size) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buffer, size, "%04d/%02d-%02d",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

// För fil: YYYY-MM-DD (Filnamn får inte ha snestreck "/")
static void GetTodayDateFile(char *buffer, size_t size) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buffer, size, "%04d-%02d-%02d",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}


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
    spotpris->entries = malloc(sizeof(SpotPriceEntry) * n);
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

// Exempel på sparad fil: spotpris_SE1_2026-01-26.json
int Spotpris_SaveToFile(const DagligSpotpris *spotpris)
{
    if (!spotpris || !spotpris->entries) return -1;

    char filename[64];
    char date_str[16];
    GetTodayDateFile(date_str, sizeof(date_str));
    snprintf(filename, sizeof(filename), "../cache_spotpris/spotpris_%s_%s.json", spotpris->area, date_str);

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
