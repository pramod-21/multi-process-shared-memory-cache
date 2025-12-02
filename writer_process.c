#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "memory_cache.h"
#include "shared_memory_manager.h"

int main()
{
    srand(time(NULL) ^ getpid());

    int shmid = shared_memory_create();
    Shared_Cache *cache = (Shared_Cache *)shared_memory_attach(shmid);
    if (cache == NULL)
    {
        printf("Writer : %d Failed to attach shared memory\n", getpid());
        exit(1);
    }
    cache_init(cache); // initialize the cache if already not initialized

    printf("Writer Process %d started", getpid());

    // writing data to cache in an infinite loop
    while (1)
    {
        int key;
        char value[VALUE_SIZE];

        printf("\nEnter key (int): ");
        scanf("%d", &key);

        printf("Enter value (string): ");
        scanf("%s", value);

        int result = cache_put(cache, key, value);

        if (result == 1)
        {
            printf("[Writer %d]: Inserted key=%d, value=%s\n", getpid(), key, value);
        }
        else if (result == 2)
        {
            printf("[Writer %d]: Updated key=%d, value=%s\n", getpid(), key, value);
        }
        else if (result == 3)
        {
            printf("[Writer %d]: Evicted an entry to insert key=%d, value=%s\n", getpid(), key, value);
        }
        else
        {
            printf("[Writer %d]: Failed to insert key=%d, value=%s\n", getpid(), key, value);
        }
        sleep(1);
    }

    return 0;
}