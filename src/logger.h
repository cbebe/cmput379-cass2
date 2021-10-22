#ifndef LOGGER_H_
#define LOGGER_H_

#include <pthread.h>
#include <stdio.h>
#include <time.h>

struct logger {
  FILE* fp;
  clock_t start;
  pthread_mutex_t lock;
};

#endif
