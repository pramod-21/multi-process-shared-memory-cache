#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "memory_cache.h"
#include "shared_memory_manager.h"

int main()
{
    // seed random number generator uniquely for each reader
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
        printf("Reader : %d Failed to attach shared memory\n", getpid());
        exit(1);
    }

    cache_init(cache); // initializing the cache if not initialized
    printf("Reader Process %d started\n", getpid());

    while (1)
    {
        char cmd[16];
        printf("Enter 'exit' to stop/quit reader %d or press Enter to continue: ", getpid());
        if (fgets(cmd, sizeof(cmd), stdin) != NULL)
        {
            if (strncmp(cmd, "exit", 4) == 0)
            {
                printf("Reader Process %d exiting... and exited :)\n", getpid());
                break;
            }
        }
        int key = rand() % 100;
        char out[VALUE_SIZE];

        printf("\n[Reader %d] Generated key=%d\n", getpid(), key);

        int hit = cache_get(cache, key, out);

        if (hit == 1)
        {
            printf("[Reader %d]: Key=%d found with value=%s\n",
                   getpid(), key, out);
        }
        else
        {
            printf("[Reader %d]: Key=%d not found in cache\n",
                   getpid(), key);
        }

        sleep(1);
    }

    shared_memory_detach(cache);
    return 0;
}
