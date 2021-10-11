#include <stdio.h>
#include <stdlib.h>

// accept one or two command line arguments
// prodcon nthreads <id>
FILE* parse_args(int* nthreads, int argc, char const* argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: prodcon N_THREADS [LOG_ID]\n");
    exit(2);
  }
  *nthreads = atoi(argv[1]);
  if (*nthreads <= 0) {
    *nthreads = 1;  // make sure that nthreads is a positive number
  }

  int log_id = 0;
  if (argc >= 3) {
    log_id = atoi(argv[2]);
  }

  char filename[64];
  if (log_id) {
    sprintf(filename, "prodcon.%d.log", log_id);
  } else {
    // use default filename if input is missing/invalid
    sprintf(filename, "prodcon.log");
  }

  printf("number of threads: %d, file name: %s\n", *nthreads, filename);

  FILE* fp = fopen(filename, "w");
  if (fp == NULL) {
    fprintf(stderr, "Error opening file");
    exit(1);
  }
  return fp;
}

int main(int argc, char const* argv[]) {
  int nthreads;
  FILE* fp = parse_args(&nthreads, argc, argv);
  fclose(fp);
  return 0;
}
