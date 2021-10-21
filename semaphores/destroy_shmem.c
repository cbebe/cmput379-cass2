#include <stdio.h>

#include "shared_memory.h"

int main(int argc, char const* argv[]) {
  if (argc != 1) {
    printf("usage - %s // no args", argv[0]);
    return -1;
  }

  if (destroy_memory_block(FILENAME)) {
    printf("Destroyed block: %s\n", FILENAME);
  } else {
    printf("Could not destroy block: %s\n", FILENAME);
  }
  return 0;
}