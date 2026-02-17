#define MODULE_NAME "TEST_ALGOINFLUENCER"
#include "../server/log/logger.h"
#include "../libs/algorithm/algoinfluencer.h"
#include <stdio.h>
#include <stdlib.h>

//gcc -o tests/test_algoinfluencer tests/main_algoinfluencer.c libs/algorithm/algoinfluencer.c xoxoldAPI/meteo.c xoxoldAPI/spotpris.c libs/utils/fetcher.c cache/cache.c server/log/logger.c -Ilibs -Ilibs/algorithm -IxoxoldAPI -Ilibs/utils -Ilibs/Cache -Iserver/log -ljansson -lcurl -lpthread
int main (){
    log_Init("data/logs/test_algoinfluencer.log");

    AlgoInfluencer_t influencer;
    algoinfluencer_Init(&influencer);

    if (algoinfluencer_LoadSpotpris(&influencer, "data/history_spotpris/20260101_20260131_SE2.json") != 0) {
        fprintf(stderr, "Failed to load spotpris data\n");
        return 1;
    }

    printf("Loaded %zu spotpris entries:\n", influencer.spotpris_valcount);

    printf("First 5 entries:\n");
    for (size_t i = 0; i < 5 && i < influencer.spotpris_valcount; i++) {
        printf("%s: %.4lf SEK/kWh\n", influencer.spotpris[i].time_start, influencer.spotpris[i].sek_per_kwh);
    }

    algoinfluencer_Cleanup(&influencer);
    return 0;
}