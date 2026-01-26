#include <stdio.h>
#include "API/spotpris.h"
#include <curl/curl.h>
#include <stdlib.h>

int main(void)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);

    const char *areas[4] = {"SE1", "SE2", "SE3", "SE4"};
    for (int i = 0; i < 4; i++) {
        DagligSpotpris data;
        int rc = Spotpris_Fetch(&data, areas[i]);
        if (rc != 0) {
            fprintf(stderr, "Failed to fetch %s: %d\n", areas[i], rc);
            continue;
        }

        rc = Spotpris_SaveToFile(&data);
        if (rc != 0) {
            fprintf(stderr, "Failed to save %s\n", areas[i]);
        }

        free(data.entries);
    }

    curl_global_cleanup();
    return 0;
}
