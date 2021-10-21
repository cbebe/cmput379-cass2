#ifndef SHARED_MEMORY
#define SHARED_MEMORY

#include <stdbool.h>

char *attach_memory_block(char *filename, int size);
bool detach_memory_block(char *block);
bool destroy_memory_block(char *filename);

#define BLOCK_SIZE 4096
#define FILENAME "writeshmem.c"

#define SEM_PRODUCER_FNAME "destroy_shmem.exe"
#define SEM_CONSUMER_FNAME "writeshmem.exe"

#endif /* SHARED_MEMORY */
