#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "job_queue.h"
#include "logger.h"

// Trans and Sleep function forward declarations
void Trans(int n);
void Sleep(int n);

// global variables for logger and queue
struct logger* logger;
struct job_queue* queue;

/**
 * Accept one or two command line arguments
 * prodcon nthreads <id>
 */
void parse_args(int* nthreads, char* filename, int argc, char const* argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s N_THREADS [LOG_ID]\n", argv[0]);
    exit(2);
  }
  *nthreads = atoi(argv[1]);
  // ensure that nthreads is a positive number
  if (*nthreads <= 0) {
    *nthreads = 1;
  }

  int log_id = argc >= 3 ? atoi(argv[2]) : 0;
  if (log_id <= 0) {
    // use default filename if input is missing/invalid
    sprintf(filename, "prodcon.log");
  } else {
    sprintf(filename, "prodcon.%d.log", log_id);
  }
}

/**
 * Unit of execution for consumer thread
 * Infinitely loops until there are no more jobs to do
 */
void* consoomer_func(void* consumer_id) {
  int id = *((int*)consumer_id);
  free(consumer_id);
  int queue_num, job;
  while (1) {
    // ask for job
    ++queue->jobs_asked[id - 1];
    log_ask(logger, id);
    queue_num = consume(queue, &job);
    // finish thread if there's no remaining work to do
    if (job == NO_MORE_JOBS) return NULL;

    // receive job
    ++queue->jobs_received[id - 1];
    log_with_q_and_arg(logger, id, queue_num, job);
    Trans(job);
    // log completed job
    ++queue->jobs_completed[id - 1];
    log_with_arg(logger, id, job);
  }
}

/**
 * Create consumer thread with given consumer ID >= 1
 */
void create_thread(pthread_t* tid, int consumer_id) {
  int* id = malloc(sizeof(*id));
  *id = consumer_id;
  pthread_create(tid, NULL, consoomer_func, id);
}

/**
 * Unit of execution for producer/main thread
 * Read work from stdin and assign work to consumers
 * Signals all consumer threads to exit at EOF
 */
void producer_func() {
  char cmd;
  int arg;
  while (scanf("%c%d\n", &cmd, &arg) >= 0) {
    if (cmd == 'S') {
      log_with_arg(logger, 0, arg);
      queue->sleep++;
      Sleep(arg);
    } else {  // cmd == 'T'
      queue->work++;
      int queue_num = produce(queue, arg);
      log_with_q_and_arg(logger, 0, queue_num, arg);
    }
  }
  end_queue(queue);
}

int main(int argc, char const* argv[]) {
  int nthreads;
  char filename[64];

  // initialize logger and job queue
  parse_args(&nthreads, filename, argc, argv);
  queue = job_queue_init(nthreads);
  logger = logger_init(filename);

  pthread_t tid[nthreads];
  for (int i = 0; i < nthreads; ++i) create_thread(&tid[i], i + 1);
  producer_func();
  for (int i = 0; i < nthreads; ++i) pthread_join(tid[i], NULL);

  print_summary(logger, queue);

  logger_destroy(logger);
  job_queue_destroy(queue);
  return 0;
}
