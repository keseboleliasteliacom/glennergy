#include "../libs/cache/cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//test cache (run from Tests/ folder):
//gcc -o test_cache test_cache.c ../libs/cache/cache.c -I../libs/cache -lpthread && ./test_cache

int main()
{
    int test_success = 0;

    // 1. Initialize cache
    Cache cache;

    printf("TEST 1: cache_Init\n");
    if (cache_Init(&cache, "test_cachefolder") == 0)
    {
        printf("[DONE]\n\n");
        test_success++;
    }
    else
    {
        printf("[FAILED]\n");
        return 1;
    }
    
    // 2. Test SET
    const char *key = "59.33_18.07";
    const char *data = "{\"temperature\":15.5,\"humidity\":65}";
    
    printf("TEST 2: cache_Set\n");
    if (cache_Set(&cache, key, data, strlen(data), 5) == 0)
    {
        printf("[DONE]\n\n");
        test_success++;
    }
    else
    {
        printf("[FAILED]\n\n");
    }
    
    // 3. Test GET immediately
    printf("TEST 3: cache_Get (immediate)\n");
    char *buffer = NULL;
    size_t size = 0;
    
    if (cache_Get(&cache, key, &buffer, &size))
    {
        printf("[DONE]\n");
        printf("  Data: %s\n", buffer);
        printf("  Size: %zu bytes\n", size);
        test_success++;
        
        if (strcmp(buffer, data) == 0)
        {
            printf("[DONE] Data matches!\n\n");
            test_success++;
        }
        else
        {
            printf("[FAILED] Data mismatch!\n\n");
        }
        
        free(buffer);
    }
    else
    {
        printf("[FAILED] Cache miss (unexpected)\n\n");
    }
    
    // 4. Test GET with non-existent key
    printf("TEST 4: cache_Get (non-existent key)\n");
    if (!cache_Get(&cache, "nonexistent", &buffer, &size))
    {
        printf("[DONE] Correctly returned miss\n\n");
        test_success++;
    }
    else
    {
        printf("[FAILED] Should have been a miss\n\n");
        free(buffer);
    }
    
    // 5. Test expiration
    printf("TEST 5: cache_Get (after expiration)\n");
    printf("  Waiting 6 seconds...\n");
    sleep(6);
    
    if (!cache_Get(&cache, key, &buffer, &size))
    {
        printf("[DONE] Correctly expired\n\n");
        test_success++;
    }
    else
    {
        printf("[FAILED] Should have expired\n\n");
        free(buffer);
    }
    
    // 6. Test TTL=0 (no expiration)
    printf("TEST 6: cache_Set with TTL=0\n");
    const char *perm_key = "permanent";
    const char *perm_data = "{\"status\":\"permanent\"}";
    
    cache_Set(&cache, perm_key, perm_data, strlen(perm_data), 0);
    sleep(2);
    
    if (cache_Get(&cache, perm_key, &buffer, &size))
    {
        printf("[DONE] Data with TTL=0 still valid\n");
        printf("  Data: %s\n\n", buffer);
        test_success++;
        free(buffer);
    }
    else
    {
        printf("[FAILED] Should not have expired\n\n");
    }

    printf("=== All Tests Complete ===\n");
    printf("Check test_cachefolder/ directory for cached files\n");

    printf("Tests passed: %d/7\n", test_success);
    
    return 0;
}
