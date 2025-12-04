#ifndef MEMORY_CACHE_H
#define MEMORY_CACHE_H

#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<semaphore.h>

#define SHARED_MEMORY_KEY 0x1234
#define CACHE_SIZE 128
#define VALUE_SIZE 256

typedef struct {
    char value[VALUE_SIZE];
    int key;
    int valid;   //0 for invalid, 1 for valid
    int lru_counter;  //for LRU
} Cache_Entry;

typedef struct {
    Cache_Entry entries[CACHE_SIZE];
    sem_t mutex;  //semaphore for synchronization
    int initialized; //avoid re-initialization by each process
}Shared_Cache;

//cache operations
void cache_init(Shared_Cache* cache);
int cache_put(Shared_Cache* cache, int key, const char* value);
int cache_get(Shared_Cache* cache, int key, char* out_value);
int cache_evict(Shared_Cache* cache); //evicting based on LRU


//synchronization functions
void cache_lock(Shared_Cache *cache);
void cache_unlock(Shared_Cache *cache);

#endif