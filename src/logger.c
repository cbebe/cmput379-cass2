#include "logger.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

/**
 * Get duration between two timeval structs in seconds
 * (with microsecond resolution)
 */
double get_duration(struct timeval s, struct timeval e);

/**
 * Prints a value with its name for the summary
 */
void print_value(struct logger* l, char const* str, int num);

/**
 * Logs to the output file ensuring that only one thread is accessing the file
 * at a time
 */
void log_to_file(struct logger* l, int id, char* str);

/**
 * Prints job totals
 * Work - total number of work received by the producer
 * Ask - total number of times the consumers asked for work
 * Receive - total number of work received by the consumers
 * Complete - total number of work completed by the consumers
 * Sleep - total number of sleep commands received by the producer
 * Thread n - total number of jobs completed by thread n
 */
void print_total_jobs(struct logger* l, struct job_queue* q);

// -- Public Functions -- //
struct logger* logger_init(char const* filename) {
  struct logger* l = malloc(sizeof(*l));
  FILE* fp = fopen(filename, "w");
  if (fp == NULL) {
    fprintf(stderr, "Error opening file\n");
    exit(1);
  }
  l->outfile_fp = fp;
  pthread_mutex_init(&l->outfile_lock, NULL);

  // start logger clock
  gettimeofday(&l->start, NULL);

  return l;
}

void logger_destroy(struct logger* l) {
  pthread_mutex_destroy(&l->outfile_lock);
  fclose(l->outfile_fp);
  free(l);
}

void log_ask(struct logger* l, int id) {
  char buf[64];
  sprintf(buf, "%5sAsk", "");
  log_to_file(l, id, buf);
}

void log_with_arg(struct logger* l, int id, int arg) {
  char buf[64];
  sprintf(buf, "%5s%-10s %6d", "", (id > 0) ? "Complete" : "Sleep", arg);
  log_to_file(l, id, buf);
}

void log_with_q_and_arg(struct logger* l, int id, int q, int arg) {
  char buf[64];
  sprintf(buf, "Q=%2d %-10s %6d", q, (id > 0) ? "Work" : "Receive", arg);
  log_to_file(l, id, buf);
}

void print_summary(struct logger* l, struct job_queue* q) {
  struct timeval end;
  gettimeofday(&end, NULL);
  /**
   * Since this is only called after joining all the threads,
   * there is no need to protect the file with a mutex lock.
   */
  fprintf(l->outfile_fp, "Summary:\n");
  print_total_jobs(l, q);
  fprintf(l->outfile_fp, "Transactions per second: %5.2f\n",
          (double)q->work / get_duration(l->start, end));
}

// --- Private Functions --- //

double get_duration(struct timeval s, struct timeval e) {
  return ((e.tv_usec - s.tv_usec) / 1000000.0f) + (e.tv_sec - s.tv_sec);
}

void print_value(struct logger* l, char const* str, int num) {
  fprintf(l->outfile_fp, "%4s%-9s %5d\n", "", str, num);
}

void log_to_file(struct logger* l, int id, char* str) {
  struct timeval end;
  gettimeofday(&end, NULL);
  double duration = get_duration(l->start, end);
  // START CS
  pthread_mutex_lock(&l->outfile_lock);
  fprintf(l->outfile_fp, "%8.3f ID=%2d %s\n", duration, id, str);
  pthread_mutex_unlock(&l->outfile_lock);
  // END CS
}

void print_total_jobs(struct logger* l, struct job_queue* q) {
  int ask = 0, receive = 0, complete = 0;
  for (int i = 0; i < q->num_consumers; ++i) {
    ask += q->jobs_asked[i];
    receive += q->jobs_received[i];
    complete += q->jobs_completed[i];
  }

  print_value(l, "Work", q->work);
  print_value(l, "Ask", ask);
  print_value(l, "Receive", receive);
  print_value(l, "Complete", complete);
  print_value(l, "Sleep", q->sleep);
  // Print jobs completed per thread
  for (int i = 0; i < q->num_consumers; ++i) {
    char buf[32];
    sprintf(buf, "Thread %2d", i + 1);
    print_value(l, buf, q->jobs_completed[i]);
  }
}