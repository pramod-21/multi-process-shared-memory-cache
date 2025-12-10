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

    // writing data to cache in an infinite loop
    while (1)
    {
        int key;
        char value[VALUE_SIZE];

        key = rand() % 100; // random key between 0 and 99
        printf("\n[Writer %d] Generated random key: %d\n", getpid(), key);

        printf("Enter value (string) or 'exit' to stop: ");

        if (fgets(value, sizeof(value), stdin) == NULL)
        {
            printf("Input error. Try again.\n");
            clearerr(stdin); //reset stdin error state so next input works
            continue;
        }

        // removing newline character from the input
        value[strcspn(value, "\n")] = '\0';

        if (strcmp(value, "exit") == 0)
        {
            printf("Writer Process %d exiting... and exited :)\n", getpid());
            break;
        }

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