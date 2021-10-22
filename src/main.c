#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "job_queue.h"
#include "logger.h"

// Remove newline rom the end of a string
#define remove_newline(line) line[strcspn(line, "\n")] = '\0'

// Forward declarations

// tands
void Trans(int n);
void Sleep(int n);

// job_queue
struct job_queue* job_queue_init(int n_consumers);
void job_queue_destroy(struct job_queue* q);
void end_queue(struct job_queue* q);
int consume(struct job_queue* q, int* queue_num);
int produce(struct job_queue* q, int job);

// logger
void log_no_arg(struct logger* l, int id, char const* event);
void log_with_arg(struct logger* l, int id, char const* event, int arg);
void log_with_q_and_arg(struct logger* l, int id, int q, char const* event,
                        int arg);
struct logger* logger_init(char const* filename);
void logger_destroy(struct logger* l);
void start_clock(struct logger* l);
void print_summary(struct logger* l, struct job_queue* q);

// logger
struct logger* logger;
// job queue
struct job_queue* queue;

/**
 * Accept one or two command line arguments
 * prodcon nthreads <id>
 */
void parse_args(int* nthreads, char* filename, int argc, char const* argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: prodcon N_THREADS [LOG_ID]\n");
    exit(2);
  }
  *nthreads = atoi(argv[1]);
  if (*nthreads <= 0) {
    *nthreads = 1;  // make sure that nthreads is a positive number
  }

  int log_id = argc >= 3 ? atoi(argv[2]) : 0;
  if (!log_id) {
    sprintf(filename, "prodcon.log");
  } else {
    sprintf(filename, "prodcon.%d.log", log_id);
    // use default filename if input is missing/invalid
  }
}

void* consooming_thread(void* thread_id) {
  int id = *((int*)thread_id);
  free(thread_id);
  int queue_num, job;
  while (1) {
    log_no_arg(logger, 1, "Ask");
    ++queue->jobs_asked[id - 1];
    job = consume(queue, &queue_num);
    // finish thread
    if (job == NO_MORE_JOBS) {
      return NULL;
    }
    ++queue->jobs_received[id - 1];
    log_with_q_and_arg(logger, id, queue_num, "Receive", job);
    Trans(job);
    ++queue->jobs_completed[id - 1];
    log_with_arg(logger, id, "Complete", job);
  }
}

int main(int argc, char const* argv[]) {
  int nthreads;
  char filename[64];
  // set up output file
  parse_args(&nthreads, filename, argc, argv);
  logger = logger_init(filename);
  queue = job_queue_init(nthreads);

  size_t in_buf_size = 16;
  char* in_buf = malloc(sizeof(*in_buf) * in_buf_size);
  if (in_buf == NULL) {
    perror("in_buf malloc");
    exit(1);
  }
  pthread_t tid[nthreads];

  for (int i = 0; i < nthreads; ++i) {
    int* id = malloc(sizeof(*id));
    *id = i + 1;
    pthread_create(&tid[i], NULL, consooming_thread, id);
  }
  start_clock(logger);
  while (getline(&in_buf, &in_buf_size, stdin) >= 0) {
    remove_newline(in_buf);
    int arg = atoi(&in_buf[1]);
    switch (in_buf[0]) {
      case 'S': {
        log_with_arg(logger, 0, "Sleep", arg);
        queue->sleep++;
        Sleep(arg);
        break;
      }
      case 'T': {
        queue->work++;
        int q = produce(queue, arg);
        log_with_q_and_arg(logger, 0, q, "Work", arg);
        break;
      }
    }
  }
  end_queue(queue);
  for (int i = 0; i < nthreads; ++i) {
    pthread_join(tid[i], NULL);
  }
  print_summary(logger, queue);

  free(in_buf);
  logger_destroy(logger);
  job_queue_destroy(queue);
  return 0;
}
