#include "../libs/utils/fetcher.h"
#include "../cache/cache.h"
#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../libs/utils/utils.h"

#include "../xoldAPI/spotpris.h"

#define MAX_BACKLOG_DATE 20260101

//gcc -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200112L -D_GNU_SOURCE -IxoldAPI -IxoldAPI/spotpris -IxoldAPI/cache -IxoldAPI/utils tests/historyspotpris.c xoldAPI/utils/fetcher.c xoldAPI/spotpris/spotpris.c xoldAPI/cache/cache.c -lcurl -ljansson -o tests/historyspotpris

int Spotpris_HistoryFetchArea(AllaSpotpriser *spotpris, time_t start_time, int num_days, SpotprisArea area);

int Spotpris_HistoryFetchArea(AllaSpotpriser *spotpris, time_t start_time, int num_days, SpotprisArea area)
{
    if (!spotpris) return -1;

    spotpris->num_intervals[area] = 0;
    const char *area_str = area_to_string(area);
    time_t real_start_time = start_time;    // to save the original start_time for filename

    // Loop through each day from start_time
    for (int day_offset = 0; day_offset < num_days; day_offset++)
    {
        sleep(2);
        time_t t = start_time + (day_offset * 86400);
        struct tm tm = *localtime(&t);
        
        char date_str[32];
        snprintf(date_str, sizeof(date_str), "%04d/%02d-%02d",
                 tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

        char url[256];
        snprintf(url, sizeof(url),
                 "https://www.elprisetjustnu.se/api/v1/prices/%s_%s.json",
                 date_str, area_str);

        printf("Fetching %s for %s (day %+d)\n", area_str, date_str, day_offset);

        CurlResponse resp;
        Curl_Initialize(&resp);

        int rc = Curl_HTTPGet(&resp, url);
        if (rc != 0) {
            fprintf(stderr, "Failed to fetch %s day %+d: %d\n", area_str, day_offset, rc);
            Curl_Dispose(&resp);
            continue;  // Skip this day if fetch fails
        }


        json_error_t error;
        json_t *root = json_loads(resp.data, 0, &error);
        Curl_Dispose(&resp);
        
        if (!root) {
            fprintf(stderr, "JSON parse error %s day %+d: %s\n", area_str, day_offset, error.text);
            continue;
        }

        if (!json_is_array(root)) {
            json_decref(root);
            continue;
        }

        size_t n_elements = json_array_size(root);
        
        
        if (spotpris->num_intervals[area] + n_elements > SPOTPRIS_ENTRIES) {
            n_elements = SPOTPRIS_ENTRIES - spotpris->num_intervals[area];
        }

        for (size_t i = 0; i < n_elements; i++)
        {
            json_t *obj = json_array_get(root, i);
            size_t idx = spotpris->num_intervals[area] + i; //dont fuck with the area only fill index

            const char *start = json_string_value(json_object_get(obj, "time_start"));
            spotpris->data[area][idx].sek_per_kwh = json_real_value(json_object_get(obj, "SEK_per_kWh"));

            strncpy(spotpris->data[area][idx].time_start, start, 
                    sizeof(spotpris->data[area][idx].time_start) - 1);
            spotpris->data[area][idx].time_start[sizeof(spotpris->data[area][idx].time_start)-1] = '\0';
        }

        spotpris->num_intervals[area] += n_elements;
        json_decref(root);
        
        printf("Loaded %zu intervals for %s day %+d (total: %zu)\n", 
               n_elements, area_str, day_offset, spotpris->num_intervals[area]);
    }

    // Save to JSON file
    json_t *output = json_array();
    
    for (size_t i = 0; i < spotpris->num_intervals[area]; i++) {
        json_t *entry = json_object();
        json_object_set_new(entry, "time_start", 
                           json_string(spotpris->data[area][i].time_start));
        json_object_set_new(entry, "sek_per_kwh", 
                           json_real(spotpris->data[area][i].sek_per_kwh));
        json_array_append_new(output, entry);
    }
    
    // Generate filename from start_time and num_days
    struct tm *start_tm = localtime(&real_start_time);
    time_t end_time = real_start_time + ((num_days - 1) * 86400);
    struct tm *end_tm = localtime(&end_time);
    
    char output_filename[256];  // Increase buffer size
    snprintf(output_filename, sizeof(output_filename), 
         "data/historicspotpris/%04d%02d%02d_%04d%02d%02d_%s.json",
             start_tm->tm_year + 1900, start_tm->tm_mon + 1, start_tm->tm_mday,
             end_tm->tm_year + 1900, end_tm->tm_mon + 1, end_tm->tm_mday,
             area_str);
    
    if (json_dump_file(output, output_filename, JSON_INDENT(2)) != 0) {
        fprintf(stderr, "Failed to write JSON file: %s\n", output_filename);
        json_decref(output);
        return -1;
    }
    
    printf("\nSaved %zu intervals to %s\n", 
           spotpris->num_intervals[area], output_filename);
    
    json_decref(output);

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("wrong argvcount\n");
        return 1;
    }
    if (strlen(argv[1]) != 8)
    {
        printf("argv[1] YYYYMMDD format pleese\n");
        return 1;
    }
        if (strlen(argv[2]) != 8)
    {
        printf("argv[2] YYYYMMDD format pleese\n");
        return 1;
    }

    char *startptr, *endptr;
    long startdate = strtol(argv[1], &startptr, 10);
    long enddate = strtol(argv[2], &endptr, 10);

    if (*startptr != '\0' || startptr == argv[1]) {
        fprintf(stderr, "Invalid start date format: %s\n", argv[1]);
        return 1;
    }
    if (*endptr != '\0' || endptr == argv[2]) {
        fprintf(stderr, "Invalid end date format: %s\n", argv[2]);
        return 1;
    }

    if (startdate > enddate) {
        fprintf(stderr, "Start date must be before or equal to end date\n");
        return 1;
    }

    time_t now = time(NULL);
    struct tm *now_tm = localtime(&now);
    long today = (now_tm->tm_year + 1900) * 10000 + (now_tm->tm_mon + 1) * 100 + now_tm->tm_mday;

    if (startdate < MAX_BACKLOG_DATE || startdate > today || enddate < MAX_BACKLOG_DATE || enddate > today) {
        fprintf(stderr, "Date must be between 2025-10-01 and today\n");
        return 1;
    }

    // Validate start date
    int start_year = startdate / 10000;
    int start_month = (startdate / 100) % 100;
    int start_day = startdate % 100;
    
    struct tm start_tm = {0};
    start_tm.tm_year = start_year - 1900;   // Years since 1900
    start_tm.tm_mon = start_month - 1;      // 0-11
    start_tm.tm_mday = start_day;           // 1-31
    start_tm.tm_hour = 12;

    time_t start_time = mktime(&start_tm);
    if (start_time == -1 || 
        start_tm.tm_year != start_year - 1900 || 
        start_tm.tm_mon != start_month - 1 || 
        start_tm.tm_mday != start_day) {
        fprintf(stderr, "Invalid start date: %s (not a real calendar date)\n", argv[1]);
        return 1;
    }

    // Validate end date
    int end_year = enddate / 10000;
    int end_month = (enddate / 100) % 100;
    int end_day = enddate % 100;
    
    struct tm end_tm = {0};
    end_tm.tm_year = end_year - 1900;
    end_tm.tm_mon = end_month - 1;
    end_tm.tm_mday = end_day;
    end_tm.tm_hour = 12;

    time_t end_time = mktime(&end_tm);
    if (end_time == -1 || 
        end_tm.tm_year != end_year - 1900 || 
        end_tm.tm_mon != end_month - 1 || 
        end_tm.tm_mday != end_day) {
        fprintf(stderr, "Invalid end date: %s (not a real calendar date)\n", argv[2]);
        return 1;
    }

    printf("Validation successful!\n");
    printf("Start: %04d-%02d-%02d\n", start_year, start_month, start_day);
    printf("End: %04d-%02d-%02d\n", end_year, end_month, end_day);
    
    double diff_seconds = difftime(end_time, start_time);
    int num_days = (int)(diff_seconds / 86400) + 1;  // +1 to include both start and end

    printf("Processing %d days from %04d-%02d-%02d to %04d-%02d-%02d\n", 
       num_days, start_year, start_month, start_day, end_year, end_month, end_day);

    AllaSpotpriser *spotpris_data = malloc(sizeof(AllaSpotpriser));
    if (!spotpris_data) {
        fprintf(stderr, "Failed to allocate memory for spotpris data\n");
        return 1;
    }
    memset(spotpris_data, 0, sizeof(AllaSpotpriser));

    int result = Spotpris_HistoryFetchArea(spotpris_data, start_time, num_days, AREA_SE1);
    
    free(spotpris_data);
    if (result != 0) {
        fprintf(stderr, "Failed to fetch and save spotpris data\n");
        return 1;
    }
    
    return 0;
}