#ifndef JOB_QUEUE_H_
#define JOB_QUEUE_H_

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

struct job_queue {
  int queue_counter;
  int num_consumers;
  pthread_mutex_t lock;  // queue lock
  sem_t empty, full;     // semaphores for blocking producers and consumers
  int* jobs_asked;       // array of jobs asked with length N_CONSUMERS
  int* jobs_received;    // array of jobs received with length N_CONSUMERS
  int* jobs_completed;   // array of jobs completed with length N_CONSUMERS
  int* job_queue;        // queue array of length 2 * N_CONSUMERS
};

#define NO_MORE_JOBS -1

#endif
