# Multi-Process Shared Memory Cache

## Description

A concurrent cache system built using SysV Shared Memory, POSIX Semaphores, and multiple Writer/Reader processes.
Implements LRU (Least Recently Used) eviction and supports multiple independent processes accessing the same shared cache.

This project demonstrates Operating Systems concepts such as:

- Inter-process communication (IPC)

- Synchronization

- Shared memory management

- Process creation and coordination

- Reader/Writer patterns

- Cache design + LRU logic

## Table of Contents

- [Overview](#overview)

- [Architecture](#architecture)

- [Features](#features)

- [Project Structure](#project-structure)

- [Shared Memory Design](#shared-memory-design)

- [Cache Layout](#cache-layout)

- [LRU Eviction Policy](#lru-eviction-policy)

- [Build Instructions](#build-instructions)

- [How to Run](#how-to-run)

- [Output](#output)

## Overview

Multi-Process Shared Memory Cache is a Linux-based project that creates a shared cache accessed concurrently by multiple writer and reader processes.
Synchronization is implemented using an inter-process semaphore stored directly inside shared memory.

Writers insert or update key–value pairs.
Readers fetch values from the same cache.

This project includes
- Shared memory
- Cache structure
- LRU eviction
- Writers
- Readers
- Clean exit handling
- No zombie processes

## Architecture
```
      +---------------------------+
      |         main.c            |
      |---------------------------|
      | Creates shared memory     |
      | Initializes cache         |
      | Spawns writer/reader      |
      +-------------+-------------+
                    |
     ------------------------------
     |                        |               
+-----------+           +-------------+          
| writer_1  |           | reader_1    |          
+-----------+           +-------------+          
       |                        |         
-------- attach to same shared memory --------
                    |
                    v
      +-------------------------------+
      |        Shared Memory          |
      |-------------------------------|
      | Cache_Entry entries[128]      |
      | Semaphore mutex               |
      | initialized flag              |
      +-------------------------------+
                    |
             LRU Replacement
```
## Features
- Shared Memory Cache

Several processes share one cache instance.

- Semaphore-Based Synchronization

Prevents concurrent modification and race conditions.

- LRU Eviction

Automatically removes the least recently used entry when cache is full.

- Multiple Reader/Writer Processes

Can run from main or separate terminals.

- Clean Exit Mechanism

Processes end safely on user exit.

## Project Structure
```
multi-process-shared-memory-cache/
│
├── headers/
│   ├── memory_cache.h
│   └── shared_memory_manager.h
│
├── memory-cache/
│   ├── memory_cache.c
│   └── shared_memory_manager.c
│
├── main.c
├── writer_process.c
├── reader_process.c
└── README.md
```

## Shared Memory Design

Shared memory stores a single instance of:

```
typedef struct {
    Cache_Entry entries[CACHE_SIZE];
    sem_t mutex;          // inter-process semaphore
    int initialized;      // 1 = already initialized
} Shared_Cache;
```


All processes attach using:
```
shmid = shmget(SHARED_MEMORY_KEY, 0, 0666);
shmat(shmid, NULL, 0);
```

## Cache Layout

Each entry contains:
```
typedef struct {
    char value[256];
    int key;
    int valid;          // 0 = empty, 1 = used
    int lru_counter;    // used for LRU
} Cache_Entry;
```

## LRU Eviction Policy

On every read/write:

entry->lru_counter++;


When the cache is full:

cache_evict() selects the entry with the smallest lru_counter.

## Build Instructions

Compile the entire project:
```
gcc main.c memory-cache/memory_cache.c memory-cache/shared_memory_manager.c \
    -Iheaders -pthread -o main

gcc writer_process.c memory-cache/memory_cache.c memory-cache/shared_memory_manager.c \
    -Iheaders -pthread -o writer_process

gcc reader_process.c memory-cache/memory_cache.c memory-cache/shared_memory_manager.c \
    -Iheaders -pthread -o reader_process
```

## How to Run
1. Start main (creates shared memory)
./main

2. Start writers (in separate terminals)
./writer_process

3. Start readers (in separate terminals)
./reader_process

## Output

## CLEANING UP SHARED MEMORY (if needed)

For Listing shared memory segments

ipcs -m

Deleting a specific shared memory ID

Example:

ipcrm -m 2