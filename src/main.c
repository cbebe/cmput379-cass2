#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Remove newline rom the end of a string
#define remove_newline(line) line[strcspn(line, "\n")] = '\0'

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

void process_input(char const* input, FILE* outfile) {
  int n = atoi(&input[1]);  // get n after the first character
  switch (input[0]) {
    case 'T':
      fprintf(outfile, "Trans %s, n = %d\n", input, n);
      break;
    case 'S':
      fprintf(outfile, "Sleep %s, n = %d\n", input, n);
      break;
    default:
      // shouldn't happen; we're guaranteed to have good input
      fprintf(stderr, "Bad input, ignoring: %s\n", input);
  }
}

int main(int argc, char const* argv[]) {
  int nthreads;
  FILE* fp = parse_args(&nthreads, argc, argv);

  size_t in_buf_size = 16;
  char* in_buf = malloc(sizeof(*in_buf) * in_buf_size);
  if (in_buf == NULL) {
    perror("in_buf malloc");
    exit(1);
  }

  while (getline(&in_buf, &in_buf_size, stdin) >= 0) {
    remove_newline(in_buf);
    process_input(in_buf, fp);
  }

  free(in_buf);
  fclose(fp);
  return 0;
}
