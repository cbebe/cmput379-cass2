#include "logger.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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