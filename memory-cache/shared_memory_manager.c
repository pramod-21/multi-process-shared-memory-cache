#include "shared_memory_manager.h"
#include <stdio.h>
#include <stdlib.h>

int shared_memory_create() {
    int shmid = shmget(SHARED_MEMORY_KEY, SHARED_MEMORY_SIZE, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed");
        exit(1);
    }
    return shmid;
}

void* shared_memory_attach(int shmid) {
    void* shmaddr = shmat(shmid, NULL, 0);
    if (shmaddr == (void*) -1) {
        perror("shmat failed");
        exit(1);
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