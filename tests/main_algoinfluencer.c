#define MODULE_NAME "TEST_ALGOINFLUENCER"
#include "../server/log/logger.h"
#include "../libs/algorithm/algoinfluencer.h"
#include <stdio.h>
#include <stdlib.h>

//gcc -o test_algoinfluencer tests/main_algoinfluencer.c libs/algorithm/algoinfluencer.c libs/xoxoldAPI/meteo.c libs/xoxoldAPI/spotpris.c libs/utils/fetcher.c libs/cache/cache.c server/log/logger.c -Ilibs -Ilibs/algorithm -Ilibs/xoxoldAPI -Ilibs/utils -Ilibs/cache -Iserver/log -ljansson -lcurl -lpthread

int main (){
    log_Init("test_algoinfluencer.log");

    AlgoInfluencer_t influencer;
    algoinfluencer_Init(&influencer);

    if (algoinfluencer_LoadSpotpris(&influencer, "data/historicspotpris/20260101_20260131_SE1.json") != 0) {
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