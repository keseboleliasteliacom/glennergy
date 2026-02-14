#include <stdio.h>
#include "../libs/API/spotpris.h"
#include "../libs/utils/fetcher.h"
#include "../libs/cache/cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <stdlib.h>


// För att enkelt testa fetch för spotpris

/* Kommentera bort koden
cd till Tests
Kompilera med: gcc -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200112L spotpris_fetchtest.c ../libs/utils/fetcher.c ../libs/API/spotpris.c -o spotpris_app -lcurl -ljansson
kör sedan ./spotpris_app
*/


int main(void)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);

    const char *areas[4] = {"SE1", "SE2", "SE3", "SE4"};
    for (int i = 0; i < 4; i++) {
        AllaSpotpriser data;
        int rc = Spotpris_Fetch(&data, areas[i]);
        if (rc != 0) {
            fprintf(stderr, "Failed to fetch %s: %d\n", areas[i], rc);
            continue;
        }

        rc = Spotpris_SaveToFile(&data);
        if (rc != 0) {
            fprintf(stderr, "Failed to save %s\n", areas[i]);
        }

        free(data); // Free the allocated data
    }

    curl_global_cleanup();
    return 0;
}