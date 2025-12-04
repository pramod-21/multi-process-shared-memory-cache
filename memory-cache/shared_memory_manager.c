#include "shared_memory_manager.h"
#include "memory_cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/shm.h>
#include <errno.h>

int shared_memory_create() {

    size_t size = sizeof(Shared_Cache);
    printf("DEBUG: sizeof(Shared_Cache) = %ld bytes\n", size);

    long page = sysconf(_SC_PAGESIZE);
    if (page <= 0) page = 4096;
    printf("DEBUG: page size = %ld bytes\n", page);

    size = ((size + page - 1) / page) * page;
    printf("DEBUG: aligned size sent to shmget = %ld bytes\n", size);

    int shmid = shmget(SHARED_MEMORY_KEY, size, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed");
        return -1;
    }

    return shmid;
}

void* shared_memory_attach(int shmid) {
    void* shmaddr = shmat(shmid, NULL, 0);
    if (shmaddr == (void*) -1) {
        perror("shmat failed");
        return (void*) -1;
    }
    return shmaddr;
}

int shared_memory_detach(void* shmaddr) {
    if (shmdt(shmaddr) < 0) {
        perror("shmdt failed");
        return -1;
    }
    return 0;
}

int shared_memory_destroy(int shmid) {
    if (shmctl(shmid, IPC_RMID, NULL) < 0) {
        perror("shmctl(IPC_RMID) failed");
        return -1;
    }
    return 0;
}
