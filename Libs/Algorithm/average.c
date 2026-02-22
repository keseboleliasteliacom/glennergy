// #define MODULE_NAME "ALGOINFLUENCER"
// #include "algoinfluencer.h"
// #include "../../server/log/logger.h"
#include <stdio.h>
#include <stdlib.h>
// #include <string.h>
// #include <sys/stat.h>
#include "average.h"
#include "../../Cache/InputCache.h"


const char *area_names[AREA_COUNT] = {"SE1", "SE2", "SE3", "SE4"}; //usch

int compare_double(const void *a, const void *b)
{
    double diff = (*(double*)a - *(double*)b);
    if (diff < 0) return -1;  // a < b
    if (diff > 0) return 1;   // a > b
    return 0;                 // a == b
}

int average_SpotprisStats(SpotStats_t *spot, InputCache_t *cache)
{
    if (!cache || !spot) {
        fprintf(stderr, "Invalid parameters\n");
        return -1;
    }

    for (int areaindx = 0; areaindx < AREA_COUNT; areaindx++)
    {
        size_t count = cache->spotpris.count[areaindx];
        
        if (count == 0) {
            fprintf(stderr, "No data for area %s\n", area_names[areaindx]);
            continue;
        }

        double sorted[96];
        double sum = 0.0;
        
        for (size_t j = 0; j < count; j++) {
            sorted[j] = cache->spotpris.data[areaindx][j].sek_per_kwh;
            sum += sorted[j];
        }
        
        qsort(sorted, count, sizeof(double), compare_double);

        double min = sorted[0];
        double max = sorted[count - 1];
        double average = sum / count;
        
        size_t mid = count / 2;
        double median = (count % 2 == 0) 
            ? (sorted[mid - 1] + sorted[mid]) / 2.0 
            : sorted[mid];

        spot->area[areaindx].min = min;
        spot->area[areaindx].max = max;
        spot->area[areaindx].average = average;
        spot->area[areaindx].median = median;
        spot->area[areaindx].q25 = sorted[count / 4];
        spot->area[areaindx].q75 = sorted[(3 * count) / 4];

        printf("\nArea %s: min=%.3f, max=%.3f, avg=%.3f, median=%.3f, q25=%.3f, q75=%.3f\n",
               area_names[areaindx],
               spot->area[areaindx].min,
               spot->area[areaindx].max,
               spot->area[areaindx].average,
               spot->area[areaindx].median,
               spot->area[areaindx].q25,
               spot->area[areaindx].q75);
    }

    return 0;
}

