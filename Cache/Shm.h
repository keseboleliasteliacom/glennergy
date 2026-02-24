#ifndef SHM_H
#define SHM_H


#include "InputCache.h"
#include <pthread.h>
#include <time.h>
#include <stdbool.h>

#define SHM_NAME "/glennergy_shm"
#define SHM_SIZE sizeof(SharedCache_t)

typedef struct {
    int home_id;

    double Predict_solar_kwh;

    double Optimal_usage_hours[24];
    int best_windows[10][2];    // 10 best windows with start and end hours
    int num_windows;

    time_t last_calculated;
    bool valid;

} AlgorithmResults_t;

typedef struct {
    InputCache_t input;

    AlgorithmResults_t results[MAX];
    size_t results_count;

    pthread_rwlock_t input_lock;
    pthread_rwlock_t results_lock;

    time_t last_input_update;
    int initialized;
} SharedCache_t;

int shm_Create();
int shm_Destroy();

SharedCache_t* shm_Attach();
int shm_Detach(SharedCache_t *shm);
// ===Input===
int shm_Lock_Input_Read(SharedCache_t *shm);
int shm_Unlock_Input_Read(SharedCache_t *shm);

int shm_Lock_Input_Write(SharedCache_t *shm);
int shm_Unlock_Input_Write(SharedCache_t *shm);
// ===Results===
int shm_Lock_Results_Read(SharedCache_t *shm);
int shm_Unlock_Results_Read(SharedCache_t *shm);

int shm_Lock_Results_Write(SharedCache_t *shm);
int shm_Unlock_Results_Write(SharedCache_t *shm);

AlgorithmResults_t* shm_GetResults(SharedCache_t *shm, int home_id);

#endif