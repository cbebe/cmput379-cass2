#include <stdio.h>
#include <string.h>

#include "shared_memory.h"

int main(int argc, char const* argv[]) {
  if (argc != 1) {
    printf("usage - %s // no args", argv[0]);
    return -1;
  }

  // grab the shared memory block
  char* block = attach_memory_block(FILENAME, BLOCK_SIZE);
  if (block == NULL) {
    printf("ERROR: couldn't get block\n");
    return -1;
  }

  while (true) {
    if (strlen(block) > 0) {
      printf("Reading: \"%s\"\n", block);
      bool done = (strcmp(block, "quit") == 0);
      block[0] = 0;
      if (done) {
        break;
      }
    }
  }
  detach_memory_block(block);

  return 0;
}