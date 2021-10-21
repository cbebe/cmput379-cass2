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
int produce(JobQueue* queue, int job);
void end_queue(JobQueue* queue);

// output file
FILE* fp;
// job queue
JobQueue* queue;

pthread_mutex_t file_lock;

clock_t start, end;

void log_to_file(int id, char* str) {
  clock_t end = clock();
  double duration = (double)(end - start) / CLOCKS_PER_SEC;
  pthread_mutex_lock(&file_lock);
  fprintf(fp, "%8.3f ID=%2d %s\n", duration, id, str);
  pthread_mutex_unlock(&file_lock);
}

void log_no_arg(int id, char const* event) {
  char buf[64];
  sprintf(buf, "%5s%-10s", "", event);
  log_to_file(id, buf);
}

void log_with_arg(int id, char const* event, int arg) {
  char buf[64];
  sprintf(buf, "%5s%-10s %6d", "", event, arg);
  log_to_file(id, buf);
}

void log_with_q_and_arg(int id, int q, char const* event, int arg) {
  char buf[64];
  sprintf(buf, "Q=%2d %-10s %6d", q, event, arg);
  log_to_file(id, buf);
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
    log_no_arg(1, "Ask");
    ++queue->jobs_asked[id - 1];
    job = consume(queue, &queue_num);
    // finish thread
    if (job == QUEUE_END) {
      return NULL;
    }
    ++queue->jobs_received[id - 1];
    log_with_q_and_arg(id, queue_num, "Receive", job);
    Trans(job);
    ++queue->jobs_completed[id - 1];
    log_with_arg(id, "Complete", job);
  }
}

void print_value(char const* str, int num) {
  fprintf(fp, "%4s%-9s %5d\n", "", str, num);
}

void print_summary(int work, int sleep) {
  fprintf(fp, "Summary:\n");
  print_value("Work", work);
  int ask = 0, receive = 0, complete = 0;
  int work_per_thread[queue->num_consumers];
  for (int i = 0; i < queue->num_consumers; ++i) {
    work_per_thread[i] = queue->jobs_completed[i];
    ask += queue->jobs_asked[i];
    receive += queue->jobs_received[i];
    complete += queue->jobs_completed[i];
  }
  print_value("Ask", ask);
  print_value("Receive", receive);
  print_value("Complete", complete);
  print_value("Sleep", sleep);
  for (int i = 0; i < queue->num_consumers; ++i) {
    char buf[32];
    sprintf(buf, "Thread %2d", i + 1);
    print_value(buf, work_per_thread[i]);
  }
  double duration = (double)(end - start) / CLOCKS_PER_SEC;
  fprintf(fp, "Transactions per second: %5.2f\n", (double)work / duration);
  if (work != complete && work != receive) {
    fprintf(fp, "ERROR! Uncompleted jobs!\n");
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
    *id = i + 1;
    pthread_create(&tid[i], NULL, consooming_thread, id);
  }
  int work = 0, sleep = 0;
  while (getline(&in_buf, &in_buf_size, stdin) >= 0) {
    remove_newline(in_buf);
    int arg = atoi(&in_buf[1]);
    switch (in_buf[0]) {
      case 'S': {
        log_with_arg(0, "Sleep", arg);
        sleep++;
        Sleep(arg);
        break;
      }
      case 'T': {
        work++;
        int q = produce(queue, arg);
        log_with_q_and_arg(0, q, "Work", arg);
        break;
      }
    }
  }
  end_queue(queue);
  end = clock();
  for (int i = 0; i < nthreads; ++i) {
    pthread_join(tid[i], NULL);
  }
  print_summary(work, sleep);

  free(in_buf);
  delete_queue(queue);
  fclose(fp);
  return 0;
}
