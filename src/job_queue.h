#ifndef JOB_QUEUE_H_
#define JOB_QUEUE_H_

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

struct job_queue {
  int* job_queue;     // job queue with length 2 * n_consumers
  int queue_counter;  // this number is protected by a mutex lock as it's used
                      // to access the queue
  pthread_mutex_t lock;  // queue lock

  sem_t full;   // semaphore for blocking producer from adding to the queue if
                // it's full
  sem_t empty;  // semaphore for blocking a consumer from taking from the queue
                // if it's empty

  int num_consumers;  // number of consumers

  int work;  // number of work received by the producer. should match the sum of
             // jobs_completed and jobs_received
  int sleep;  // number of sleeps received by the producer

  /**
   * Arrays of length n_consumers for keeping track of asks, received jobs, and
   * completed jobs. Doesn't need to be locked since each thread has their own
   * element in each array.
   */
  int* jobs_asked;
  int* jobs_received;
  int* jobs_completed;
};

#define NO_MORE_JOBS -1

#endif
