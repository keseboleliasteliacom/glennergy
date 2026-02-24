#define _XOPEN_SOURCE 700
#define MODULE_NAME "SHM"
#include "../Server/Log/Logger.h"
#include "Shm.h"


#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int shm_Create() 
{
    LOG_INFO("Creating shared memory...");
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR | O_EXCL, 0666);
    if (shm_fd < 0) {
        if (errno == EEXIST) {
            LOG_WARNING("shm_open() Shared memory exists, recreating");
            shm_unlink(SHM_NAME);
            shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR | O_EXCL, 0666);
        }
        if (shm_fd < 0) {
            LOG_ERROR("shm_open() failed: %s", strerror(errno));
            return -1;
        }
    }
    LOG_DEBUG("Shm setting size to %zu bytes", sizeof(SharedCache_t));
    if (ftruncate(shm_fd, sizeof(SharedCache_t)) < 0) {
        LOG_ERROR("ftruncate() failed: %s", strerror(errno));
        close(shm_fd);
        shm_unlink(SHM_NAME);
        return -1;
    }
    LOG_DEBUG("Mapping shared memory...");
    SharedCache_t *shm_ptr = mmap(NULL, sizeof(SharedCache_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    close(shm_fd);

    if (shm_ptr == MAP_FAILED) {
        LOG_ERROR("mmap() failed: %s", strerror(errno));
        shm_unlink(SHM_NAME);
        return -1;
    }

    memset(shm_ptr, 0, sizeof(SharedCache_t));
    LOG_DEBUG("Initializing locks...");

    pthread_rwlockattr_t attr;
    pthread_rwlockattr_init(&attr);
    pthread_rwlockattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);

    pthread_rwlock_init(&shm_ptr->input_lock, &attr);
    pthread_rwlock_init(&shm_ptr->results_lock, &attr);

    pthread_rwlockattr_destroy(&attr);

    shm_ptr->initialized = 1;
    shm_ptr->last_input_update = time(NULL);

    LOG_DEBUG("Unmapping shared memory from this process...");
    munmap(shm_ptr, sizeof(SharedCache_t));

    LOG_INFO("Shared memory created (%zu bytes)", sizeof(SharedCache_t));
    return 0;
}

int shm_Destroy()
{
    LOG_INFO("Destroying shared memory...");
    if (shm_unlink(SHM_NAME) < 0) {
        LOG_ERROR("shm_unlink() failed: %s", strerror(errno));
        return -1;
    }
    LOG_INFO("Shared memory destroyed");
    return 0;
}

SharedCache_t* shm_Attach() 
{
    LOG_INFO("Attaching to shared memory...");

    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd < 0) {
        LOG_ERROR("shm_open() failed: %s", strerror(errno));
        return NULL;
    }
    LOG_DEBUG("Mapping shared memory...");
    SharedCache_t *shm_ptr = mmap(NULL, sizeof(SharedCache_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    close(shm_fd);

    if (shm_ptr == MAP_FAILED) {
        LOG_ERROR("mmap() failed: %s", strerror(errno));
        return NULL;
    }

    if (!shm_ptr->initialized) {
        LOG_ERROR("Shared memory not initialized");
        munmap(shm_ptr, sizeof(SharedCache_t));
        return NULL;
    }
    LOG_INFO("Attached to shared memory");
    return shm_ptr;
}

int shm_Detach(SharedCache_t *shm)
{
    LOG_INFO("Detaching from shared memory");
    if (!shm) {
        LOG_WARNING("shm_Detach() called with NULL pointer");
        return -1;
    }
    if (munmap(shm, sizeof(SharedCache_t)) < 0) {
        LOG_ERROR("munmap() failed: %s", strerror(errno));
        return -1;
    }
    LOG_INFO("Detached from shared memory");
    return 0;
}

// ===Input read===
int shm_Lock_Input_Read(SharedCache_t *shm)
{   LOG_INFO("Locking input for read...");
    if (!shm) return -1;
    return pthread_rwlock_rdlock(&shm->input_lock);
}
int shm_Unlock_Input_Read(SharedCache_t *shm)
{   LOG_INFO("Unlocking input read lock...");
    if (!shm) return -1;
    return pthread_rwlock_unlock(&shm->input_lock);
}
// ===Input write===
int shm_Lock_Input_Write(SharedCache_t *shm)
{   LOG_INFO("Locking input for write...");
    if (!shm) return -1;
    return pthread_rwlock_wrlock(&shm->input_lock);
}
int shm_Unlock_Input_Write(SharedCache_t *shm)
{   LOG_INFO("Unlocking input write lock...");
    if (!shm) return -1;
    return pthread_rwlock_unlock(&shm->input_lock);
}

// ===Results read===
int shm_Lock_Results_Read(SharedCache_t *shm)
{   LOG_INFO("Locking results for read...");
    if (!shm) return -1;
    return pthread_rwlock_rdlock(&shm->results_lock);
}
int shm_Unlock_Results_Read(SharedCache_t *shm)
{   LOG_INFO("Unlocking results read lock...");
    if (!shm) return -1;
    return pthread_rwlock_unlock(&shm->results_lock);
}
// ===Results write===
int shm_Lock_Results_Write(SharedCache_t *shm)
{   LOG_INFO("Locking results for write...");
    if (!shm) return -1;
    return pthread_rwlock_wrlock(&shm->results_lock);
}
int shm_Unlock_Results_Write(SharedCache_t *shm)
{   LOG_INFO("Unlocking results write lock...");
    if (!shm) return -1;
    return pthread_rwlock_unlock(&shm->results_lock);
}   

// ===Helper to get results for a specific home_id===
AlgorithmResults_t* shm_GetResults(SharedCache_t *shm, int home_id)
{
    LOG_INFO("Getting results for home_id %d", home_id);
    if (!shm)
        return NULL;

    for (size_t i = 0; i < shm->results_count; i++) {
        if (shm->results[i].home_id == home_id) {
            LOG_INFO("Found results for home_id %d", home_id);
            return &shm->results[i];
        }
    }
    LOG_WARNING("No results found for home_id %d", home_id);
    return NULL;
}