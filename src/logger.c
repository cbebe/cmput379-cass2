#include "logger.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "job_queue.h"

void log_to_file(struct logger* l, int id, char* str) {
  clock_t end = clock();
  double duration = (double)(end - l->start) / CLOCKS_PER_SEC;
  pthread_mutex_lock(&l->lock);
  fprintf(l->fp, "%8.3f ID=%2d %s\n", duration, id, str);
  pthread_mutex_unlock(&l->lock);
}

void log_no_arg(struct logger* l, int id, char const* event) {
  char buf[64];
  sprintf(buf, "%5s%-10s", "", event);
  log_to_file(l, id, buf);
}

void log_with_arg(struct logger* l, int id, char const* event, int arg) {
  char buf[64];
  sprintf(buf, "%5s%-10s %6d", "", event, arg);
  log_to_file(l, id, buf);
}

void log_with_q_and_arg(struct logger* l, int id, int q, char const* event,
                        int arg) {
  char buf[64];
  sprintf(buf, "Q=%2d %-10s %6d", q, event, arg);
  log_to_file(l, id, buf);
}

struct logger* logger_init(char const* filename) {
  struct logger* l = malloc(sizeof(*l));
  FILE* fp = fopen(filename, "w");
  if (fp == NULL) {
    fprintf(stderr, "Error opening file\n");
    exit(1);
  }
  l->fp = fp;
  pthread_mutex_init(&l->lock, NULL);

  return l;
}

void start_clock(struct logger* l) { l->start = clock(); }

void logger_destroy(struct logger* l) {
  pthread_mutex_destroy(&l->lock);
  fclose(l->fp);
  free(l);
}

inline void print_value(struct logger* l, char const* str, int num) {
  fprintf(l->fp, "%4s%-9s %5d\n", "", str, num);
}

void print_summary(struct logger* l, struct job_queue* q) {
  clock_t end = clock();
  fprintf(l->fp, "Summary:\n");
  print_value(l, "Work", q->work);
  int ask = 0, receive = 0, complete = 0;
  int work_per_thread[q->num_consumers];
  for (int i = 0; i < q->num_consumers; ++i) {
    work_per_thread[i] = q->jobs_completed[i];
    ask += q->jobs_asked[i];
    receive += q->jobs_received[i];
    complete += q->jobs_completed[i];
  }
  print_value(l, "Ask", ask);
  print_value(l, "Receive", receive);
  print_value(l, "Complete", complete);
  print_value(l, "Sleep", q->sleep);
  for (int i = 0; i < q->num_consumers; ++i) {
    char buf[32];
    sprintf(buf, "Thread %2d", i + 1);
    print_value(l, buf, work_per_thread[i]);
  }
  double duration = (double)(end - l->start) / CLOCKS_PER_SEC;
  fprintf(l->fp, "Transactions per second: %5.2f\n",
          (double)q->work / duration);
  // something went really wrong!!
  if (q->work != complete && q->work != receive) {
    fprintf(l->fp, "ERROR! Uncompleted jobs!\n");
  }
}