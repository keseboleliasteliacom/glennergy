#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include "../Server/Log/Logger.h"

//gcc -o Tests/test_logger_multithread Tests/test_logger_multithread.c Server/Log/Logger.c -pthread -I.
//./Tests/test_logger_multithread
//cat Logs/log.txt | grep -E "Thread|Main" | tail -30

typedef struct {
    int thread_id;
    int message_count;
} ThreadArgs;

void* worker_thread(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    char module[32];
    snprintf(module, sizeof(module), "Thread%d", args->thread_id);
    
    for (int i = 0; i < args->message_count; i++) {
        char msg[256];
        snprintf(msg, sizeof(msg), "Message %d from thread %d (pthread_id: %lu)", 
                 i, args->thread_id, pthread_self());
        LOG_INFO(module, msg);
        usleep(1);
    }
    
    printf("Thread %d done\n", args->thread_id);
    return NULL;
}

int main() {
    printf("=== Multi-Thread Logger Test ===\n");
    
    if (log_Init() < 0) {
        fprintf(stderr, "Failed to init logger\n");
        return 1;
    }
    
    printf("Logger initialized, spawning threads...\n");
    
    struct timeval start, end;
    gettimeofday(&start, NULL);
    
    // Create 20 worker threads (matching your thread pool size)
    const int NUM_THREADS = 20;
    pthread_t threads[NUM_THREADS];
    ThreadArgs args[NUM_THREADS];
    
    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].thread_id = i;
        args[i].message_count = 1000;
        
        if (pthread_create(&threads[i], NULL, worker_thread, &args[i]) != 0) {
            perror("pthread_create");
            continue;
        }
    }
    
    // Main thread logs too
    for (int i = 0; i < 10; i++) {
        LOG_INFO("MainThread", "Message from main thread");
        usleep(5000);
    }
    
    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    
    printf("All threads done, cleaning up...\n");
    printf("Total time: %.3f seconds\n", elapsed);
    printf("Throughput: %.0f messages/second\n", (NUM_THREADS * 1000 + 10) / elapsed);
    sleep(1);
    log_Cleanup();
    
    printf("=== Multi-Thread Test Complete ===\n");
    printf("Expected ~%d messages (20 threads × 1000 + 10 main)\n", NUM_THREADS * 1000 + 10);
    
    return 0;
}
