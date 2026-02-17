#define _POSIX_C_SOURCE 200809L
#define MODULE_NAME "TEST_CACHE"
#include "../server/log/logger.h"
#include "../cache/cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

//gcc -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L tests/test_cache.c cache/cache.c server/log/logger.c -Icache -Iserver/log -lpthread -o tests/test_cache
#define TEST_DIR "tests/test_cachefolder"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_RED "\x1b[31m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_RESET "\x1b[0m"

static int tests_passed = 0;
static int tests_failed = 0;

void test_assert(const char *test_name, bool condition, const char *message)
{
    if (condition) {
        printf(ANSI_GREEN "✓ PASS" ANSI_RESET " %s: %s\n", test_name, message);
        tests_passed++;
    } else {
        printf(ANSI_RED "✗ FAIL" ANSI_RESET " %s: %s\n", test_name, message);
        tests_failed++;
    }
}

void test_section(const char *section_name)
{
    printf("\n" ANSI_YELLOW "═══ %s ═══" ANSI_RESET "\n", section_name);
}

bool file_is_pure_json(const char *filepath)
{
    FILE *f = fopen(filepath, "r");
    if (!f) return false;
    
    char first_char = fgetc(f);
    fclose(f);
    
    // Pure JSON should start with { or [
    return (first_char == '{' || first_char == '[');
}

int main()
{
    log_Init(NULL);
    
    printf("\n" ANSI_YELLOW "╔═══════════════════════════════════╗\n");
    printf("║     CACHE MODULE TEST SUITE       ║\n");
    printf("╚═══════════════════════════════════╝" ANSI_RESET "\n");

    Cache cache;
    
    // ═══════════════════════════════════════════════════════
    test_section("TEST 1: Initialization");
    // ═══════════════════════════════════════════════════════
    
    int init_result = cache_Init(&cache, TEST_DIR, 3);  // 3 second TTL for testing
    test_assert("Init", init_result == 0, "cache_Init succeeded");
    test_assert("Init", cache.cache_dir != NULL, "cache_dir was allocated");
    test_assert("Init", cache.ttl == 3, "TTL set correctly (3 seconds)");
    
    if (init_result != 0) {
        printf("\n" ANSI_RED "FATAL: Cannot continue without cache initialization" ANSI_RESET "\n");
        return 1;
    }

    // ═══════════════════════════════════════════════════════
    test_section("TEST 2: Cache Set (Write)");
    // ═══════════════════════════════════════════════════════
    
    const char *key1 = "test_data_1";
    const char *json1 = "{\"temperature\":15.5,\"humidity\":65,\"location\":\"Stockholm\"}";
    
    int set_result = cache_Set(&cache, key1, json1, strlen(json1));
    test_assert("Set", set_result == 0, "cache_Set succeeded");
    
    // Verify file exists
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s.json", TEST_DIR, key1);
    struct stat st;
    test_assert("Set", stat(filepath, &st) == 0, "Cache file created on disk");
    test_assert("Set", file_is_pure_json(filepath), "File contains pure JSON (no TTL prefix)");

    // ═══════════════════════════════════════════════════════
    test_section("TEST 3: Cache Get (Read)");
    // ═══════════════════════════════════════════════════════
    
    char *buffer = NULL;
    size_t size = 0;
    
    bool get_result = cache_Get(&cache, key1, &buffer, &size);
    test_assert("Get", get_result == true, "cache_Get succeeded");
    test_assert("Get", buffer != NULL, "Buffer allocated");
    test_assert("Get", size == strlen(json1), "Size matches original");
    test_assert("Get", strcmp(buffer, json1) == 0, "Data integrity verified");
    
    free(buffer);
    buffer = NULL;

    // ═══════════════════════════════════════════════════════
    test_section("TEST 4: Cache Validity Check");
    // ═══════════════════════════════════════════════════════
    
    bool is_valid = cache_IsValid(&cache, key1);
    test_assert("IsValid", is_valid == true, "Freshly cached data is valid");
    
    bool invalid_key = cache_IsValid(&cache, "nonexistent_key");
    test_assert("IsValid", invalid_key == false, "Non-existent key is invalid");

    // ═══════════════════════════════════════════════════════
    test_section("TEST 5: Multiple Entries");
    // ═══════════════════════════════════════════════════════
    
    const char *key2 = "weather_gothenburg";
    const char *json2 = "{\"temp\":12.3,\"wind\":5.2}";
    
    cache_Set(&cache, key2, json2, strlen(json2));
    
    bool get2 = cache_Get(&cache, key2, &buffer, &size);
    test_assert("Multiple", get2 == true, "Second entry retrieved");
    test_assert("Multiple", strcmp(buffer, json2) == 0, "Second entry data correct");
    free(buffer);
    buffer = NULL;
    
    // First entry still valid
    bool get1_again = cache_Get(&cache, key1, &buffer, &size);
    test_assert("Multiple", get1_again == true, "First entry still valid");
    free(buffer);
    buffer = NULL;

    // ═══════════════════════════════════════════════════════
    test_section("TEST 6: TTL Expiration");
    // ═══════════════════════════════════════════════════════
    
    printf("Waiting 4 seconds for TTL to expire...\n");
    sleep(4);
    
    bool expired_valid = cache_IsValid(&cache, key1);
    test_assert("Expiration", expired_valid == false, "cache_IsValid detects expiration");
    
    bool expired_get = cache_Get(&cache, key1, &buffer, &size);
    test_assert("Expiration", expired_get == false, "cache_Get rejects expired data");

    // ═══════════════════════════════════════════════════════
    test_section("TEST 7: Cache with No TTL (TTL=0)");
    // ═══════════════════════════════════════════════════════
    
    Cache perm_cache;
    cache_Init(&perm_cache, TEST_DIR, 0);  // TTL = 0 means no expiration
    
    const char *perm_key = "permanent_data";
    const char *perm_json = "{\"status\":\"never_expires\"}";
    
    cache_Set(&perm_cache, perm_key, perm_json, strlen(perm_json));
    sleep(2);
    
    bool perm_valid = cache_IsValid(&perm_cache, perm_key);
    test_assert("No TTL", perm_valid == true, "Data with TTL=0 never expires");
    
    bool perm_get = cache_Get(&perm_cache, perm_key, &buffer, &size);
    test_assert("No TTL", perm_get == true, "cache_Get works with TTL=0");
    free(buffer);
    buffer = NULL;
    
    cache_Dispose(&perm_cache);

    // ═══════════════════════════════════════════════════════
    test_section("TEST 8: Large Data");
    // ═══════════════════════════════════════════════════════

    // Allocate on heap instead of stack
    char *large_json = malloc(11000);
    if (!large_json) {
        printf("Failed to allocate large test buffer\n");
        goto skip_large_test;
    }

    snprintf(large_json, 11000, "{\"data\":\"");
    for (int i = 0; i < 10000; i++) {
        large_json[strlen(large_json)] = 'A' + (i % 26);
    }
    strcat(large_json, "\"}");

    const char *large_key = "large_data";
    cache_Set(&cache, large_key, large_json, strlen(large_json));

    bool large_get = cache_Get(&cache, large_key, &buffer, &size);
    test_assert("Large Data", large_get == true, "Large data cached successfully");
    test_assert("Large Data", size == strlen(large_json), "Large data size correct");
    test_assert("Large Data", strcmp(buffer, large_json) == 0, "Large data integrity verified");
    free(buffer);
    buffer = NULL;
    free(large_json);  // Free the test buffer

    skip_large_test:
    // ═══════════════════════════════════════════════════════
    test_section("TEST 9: Edge Cases");
    // ═══════════════════════════════════════════════════════
    
    // Empty JSON object
    cache_Set(&cache, "empty", "{}", 2);
    bool empty_get = cache_Get(&cache, "empty", &buffer, &size);
    test_assert("Edge", empty_get == true, "Empty JSON object cached");
    free(buffer);
    buffer = NULL;
    
    // Unicode in JSON
    const char *unicode_json = "{\"city\":\"Malmö\",\"temp\":\"15°C\"}";
    cache_Set(&cache, "unicode", unicode_json, strlen(unicode_json));
    bool unicode_get = cache_Get(&cache, "unicode", &buffer, &size);
    test_assert("Edge", unicode_get == true, "Unicode data cached");
    test_assert("Edge", strcmp(buffer, unicode_json) == 0, "Unicode data integrity");
    free(buffer);
    buffer = NULL;

    // ═══════════════════════════════════════════════════════
    test_section("TEST 10: Cleanup");
    // ═══════════════════════════════════════════════════════
    
    cache_Dispose(&cache);
    test_assert("Cleanup", cache.cache_dir == NULL, "cache_Dispose freed cache_dir");

    // ═══════════════════════════════════════════════════════
    // Summary
    // ═══════════════════════════════════════════════════════
    
    printf("\n" ANSI_YELLOW "╔═══════════════════════════════════╗\n");
    printf("║         TEST SUMMARY              ║\n");
    printf("╚═══════════════════════════════════╝" ANSI_RESET "\n");
    printf(ANSI_GREEN "  Passed: %d" ANSI_RESET "\n", tests_passed);
    printf(ANSI_RED "  Failed: %d" ANSI_RESET "\n", tests_failed);
    printf("  Total:  %d\n", tests_passed + tests_failed);
    
    if (tests_failed == 0) {
        printf("\n" ANSI_GREEN "✓ ALL TESTS PASSED!" ANSI_RESET "\n");
    } else {
        printf("\n" ANSI_RED "✗ SOME TESTS FAILED" ANSI_RESET "\n");
    }
    
    printf("\nCache files location: %s/\n", TEST_DIR);
    
    log_Cleanup();
    
    return (tests_failed == 0) ? 0 : 1;
}