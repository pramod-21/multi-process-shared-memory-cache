#include "memory_cache.h"
#include "shared_memory_manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig) {
    keep_running = 0;
    printf("\nMain received Ctrl+C. Shutting down...\n");
}

int main() {

    signal(SIGINT, handle_sigint);

    int shmid = shared_memory_create();
    if (shmid < 0) {
        printf("Main: Failed to create shared memory\n");
        exit(1);
    }

    Shared_Cache *cache = (Shared_Cache *)shared_memory_attach(shmid);
    if (cache == (void*) -1) {
        printf("Main: Failed to attach to the shared memory\n");
        exit(1);
    }

    cache_init(cache);

    printf("\n\nMain Process %d: Shared memory ready.\n", getpid());
    printf("Press Ctrl+C anytime to exit.\n");

    //writer processes
    for (int i = 0; i < 2; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            freopen("/dev/null", "r", stdin);
            freopen("/dev/null", "w", stdout);
            freopen("/dev/null", "w", stderr);

            execl("./writer_process", "writer_process", NULL);
            perror("execl writer failed");
            _exit(1);
        }
        printf("Main: Started writer %d\n", pid);
    }

    //reader processes
    for (int i = 0; i < 2; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            freopen("/dev/null", "r", stdin);
            freopen("/dev/null", "w", stdout);
            freopen("/dev/null", "w", stderr);

            execl("./reader_process", "reader_process", NULL);
            perror("execl reader failed");
            _exit(1);
        }
        printf("Main: Started reader %d\n", pid);
    }
    printf("\nMain: All child processes started. use seprate terminals to run reader and writer processes\n");
    //main is waiting for Ctrl+C
    while (keep_running) {
        sleep(1);
    }

    printf("Main cleaning up...\n");

    //detach and destroying shared memory
    shared_memory_detach(cache);
    shared_memory_destroy(shmid);

    printf("Main exited.\n");
    return 0;
}
