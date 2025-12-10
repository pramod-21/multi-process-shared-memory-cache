#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "memory_cache.h"
#include "shared_memory_manager.h"

int main()
{
    // seed random number generator uniquely for each writer
    srand(time(NULL) ^ getpid());

    int shmid = shmget(SHARED_MEMORY_KEY, 0, 0666);
    if (shmid < 0)
    {
        perror("shmget (reader) failed");
        exit(1);
    }

    Shared_Cache *cache = (Shared_Cache *)shared_memory_attach(shmid);
    if (cache == (void *)-1)
    {
        fprintf(stderr, "Writer : %d Failed to attach shared memory\n", getpid());
        exit(1);
    }
    cache_init(cache); // initialize the cache if already not initialized

    printf("Writer Process %d started", getpid());

    int key;
    char value[VALUE_SIZE];
    // writing data to cache in an infinite loop
    while (1)
    {
        printf("\n[Writer %d] Enter key (integer): ", getpid());
        if (scanf("%d", &key) != 1)
        {
            printf("Invalid input. Please enter an integer for key.\n");
            while (getchar() != '\n')
                ; // clear invalid input
            continue;
        }

        printf("Enter value (no spaces, or 'exit' to quit): ");
        scanf("%s", value);

        if (strcmp(value, "exit") == 0)
            break;

        int result = cache_put(cache, key, value);
        /*1 for insert
          2 for update
          3 for evict and insert
        */
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
    shared_memory_detach(cache);
    return 0;
}