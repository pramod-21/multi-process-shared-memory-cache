#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "memory_cache.h"
#include "shared_memory_manager.h"

int main()
{
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
    cache_init(cache); // initializing the cache
    printf("Reader Process %d started", getpid());

    while (1)
    {
        int key;
        char out[VALUE_SIZE];

        printf("\nEnter key to read (int) or -1 to exit: ");
        if (scanf("%d", &key) != 1)
        {
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
            {
            }
            continue;
        }
        if (key == -1)
        {
            printf("reader %d exiting...\n", getpid());
            break;
        }

        int hit = cache_get(cache, key, out);
        if (hit == 1)
        {
            printf("[Reader %d]: Key=%d found with value=%s\n", getpid(), key, out);
        }
        else
        {
            printf("[Reader %d]: Key=%d not found in cache\n", getpid(), key);
        }

        sleep(1);
    }
    shared_memory_detach(cache);
    return 0;
}