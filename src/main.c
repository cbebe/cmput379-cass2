#include <stdio.h>
#include <stdlib.h>

void parse_args(int* n_threads, char* filename, int argc, char const* argv[]) {
  if (argc < 2) {
    // accept one or two command line arguments
    // prodcon nthreads <id>
    fprintf(stderr, "Usage: prodcon N_THREADS [LOG_ID]\n");
    exit(2);
  }
  *n_threads = atoi(argv[1]);
  int log_id = 0;
  if (argc >= 3) {
    log_id = atoi(argv[2]);
  }
  if (log_id) {
    sprintf(filename, "prodcon.%d.log", log_id);
  } else {
    sprintf(filename, "prodcon.log");
  }

  printf("number of threads: %d, file name: %s\n", *n_threads, filename);
}

int main(int argc, char const* argv[]) {
  int n_threads;
  char filename[64];
  parse_args(&n_threads, filename, argc, argv);
  return 0;
}
