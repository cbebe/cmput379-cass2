#include "main.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Remove newline rom the end of a string
#define remove_newline(line) line[strcspn(line, "\n")] = '\0'

// Forward declarations
void Trans(int n);
void Sleep(int n);

JobQueue* new_queue(int n_consumers);
void delete_queue(JobQueue* queue);
int consume(JobQueue* queue, int* queue_num);
void produce(JobQueue* queue, int job);
void end_queue(JobQueue* queue);

// output file
FILE* fp;
// job queue
JobQueue* queue;

pthread_mutex_t file_lock;

clock_t start;

void log(FILE* outfile, char const* str) {
  clock_t end = clock();
  pthread_mutex_lock(&file_lock);
  fprintf(outfile, "%3.3f %s\n", (double)(end - start) / CLOCKS_PER_SEC, str);
  pthread_mutex_unlock(&file_lock);
}

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

  FILE* fp = fopen(filename, "w");
  if (fp == NULL) {
    fprintf(stderr, "Error opening file");
    exit(1);
  }
  return fp;
}

void* consooming_thread(void* thread_id) {
  int id = *((int*)thread_id);
  free(thread_id);
  int queue_num, job;
  while (1) {
    // finish thread
    job = consume(queue, &queue_num);
    if (job == QUEUE_END) {
      return NULL;
    }
    Trans(job);
    ++queue->jobs_completed[id - 1];
  }
}

void process_input(char const* input) {
  int arg = atoi(&input[1]);
  switch (input[0]) {
    case 'S':
      Sleep(arg);
      break;
    case 'T':
      produce(queue, arg);
      break;
  }
}

int main(int argc, char const* argv[]) {
  int nthreads;
  // set up output file
  fp = parse_args(&nthreads, argc, argv);
  queue = new_queue(nthreads);

  size_t in_buf_size = 16;
  char* in_buf = malloc(sizeof(*in_buf) * in_buf_size);
  if (in_buf == NULL) {
    perror("in_buf malloc");
    exit(1);
  }
  pthread_t tid[nthreads];
  start = clock();
  for (int i = 0; i < nthreads; ++i) {
    int* id = malloc(sizeof(*id));
    *id = i;
    pthread_create(&tid[i], NULL, consooming_thread, id);
  }
  while (getline(&in_buf, &in_buf_size, stdin) >= 0) {
    remove_newline(in_buf);
    process_input(in_buf);
  }
  end_queue(queue);
  for (int i = 0; i < nthreads; ++i) {
    pthread_join(tid[i], NULL);
  }

  free(in_buf);
  delete_queue(queue);
  fclose(fp);
  return 0;
}
