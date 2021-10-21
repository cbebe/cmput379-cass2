#include <stdio.h>
#include <string.h>

#include "shared_memory.h"

#define NUM_ITERATIONS 10

int main(int argc, char const *argv[]) {
  if (argc != 2) {
    printf("usage - %s [stuff to write]\n", argv[0]);
    return -1;
  }

  // grab the shared memory block
  char *block = attach_memory_block(FILENAME, BLOCK_SIZE);
  if (block == NULL) {
    printf("ERROR: couldn't get block\n");
    return -1;
  }

  for (int i = 0; i < NUM_ITERATIONS; ++i) {
    printf("Writing: \"%s\"\n", argv[1]);
    strncpy(block, argv[1], BLOCK_SIZE);
  }

  detach_memory_block(block);

  return 0;
}
