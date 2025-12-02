#include "memory_cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void cache_lock(Shared_Cache *cache) {
    sem_wait(&cache->mutex);
}

void cache_unlock(Shared_Cache *cache) {
    sem_post(&cache->mutex);
}

void cache_init(Shared_Cache* cache) {
    if(cache->initialized == 1) {
        return; //do nothing just exit because already initialized
    }

    sem_init(&cache->mutex, 1, 1); //initialize semaphore for synchronization

    cache_lock(cache);

    if(!cache->initialized) {
        for(int i = 0; i < CACHE_SIZE; i++) {
            cache->entries[i].valid = 0; //marks entries as invalid/empty
            cache->entries[i].lru_counter = 0; //initializing LRU counter
        }
        cache->initialized = 1; //mark as initialized
    }
    cache_unlock(cache);
    return;
}

int cache_evict(Shared_Cache* cache) {
    int lru_index = -1;
    int min_lru = 999999;

    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache->entries[i].valid == 1 &&
            cache->entries[i].lru_counter < min_lru) 
        {
            min_lru = cache->entries[i].lru_counter;
            lru_index = i;
        }
    }

    if (lru_index != -1) {
        cache->entries[lru_index].valid = 0;
        cache->entries[lru_index].key = 0;
        cache->entries[lru_index].lru_counter = 0;
        cache->entries[lru_index].value[0] = '\0';

        return lru_index;
    }

    return -1; // nothing to evict
}

int cache_put(Shared_Cache* cache, int key, const char* value) {
    cache_lock(cache);

    //updating existing entry
    for(int i = 0; i < CACHE_SIZE; i++) {
        if(cache->entries[i].valid == 1 && cache->entries[i].key == key) {
            strcpy(cache->entries[i].value, value);
            cache->entries[i].lru_counter++;
            cache_unlock(cache);
            return 2; //updated existing entry
        }
    }

    //finding an empty slot
    for(int i = 0; i < CACHE_SIZE; i++) {
        if(cache->entries[i].valid == 0) {
            cache->entries[i].valid = 1;
            cache->entries[i].key = key;
            strcpy(cache->entries[i].value, value);
            cache->entries[i].lru_counter = 1;
            cache_unlock(cache);
            return 1; //inserted new entry
        }
    }

    //no empty slot have to evict
    int evict_idx = cache_evict(cache);  //calling cache_evict function
    if(evict_idx != -1) {
        cache->entries[evict_idx].valid = 1;
        cache->entries[evict_idx].key = key;
        snprintf(cache->entries[evict_idx].value, VALUE_SIZE, "%s", value);
        cache->entries[evict_idx].lru_counter = 1;
        cache_unlock(cache);
        return 3; //inserted after eviction
    }

    cache_unlock(cache);
    return -1; //cache full and eviction failed
}

int cache_get(Shared_Cache* cache, int key) {
    cache_lock(cache);

    for(int i = 0; i < CACHE_SIZE; i++) {
        if(cache->entries[i].valid == 1 && cache->entries[i].key == key) {
            cache->entries[i].lru_counter++;
            printf("Value: %s\n", cache->entries[i].value);
            cache_unlock(cache);
            return 0; //Hit(found)
        }
    }

    cache_unlock(cache);
    return -1; //Miss(not found)
}
