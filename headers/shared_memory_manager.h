#ifndef SHARED_MEMORY_MANAGER_H
#define SHARED_MEMORY_MANAGER_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHARED_MEMORY_KEY 0x1234
#define SHARED_MEMORY_SIZE 4096

int shared_memory_create();
void* shared_memory_attach(int shmid);
int shared_memory_detach(void* shmaddr);
int shared_memory_destroy(int shmid);

#endif