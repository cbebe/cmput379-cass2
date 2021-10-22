#include "job_queue.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

/**
 * Initialize a job queue
 */
struct job_queue* job_queue_init(int n_consumers) {
  struct job_queue* q = malloc(sizeof(*q));
  q->queue_counter = 0;
  q->work = 0;
  q->sleep = 0;
  // The q must be able to hold 2 x #consumers amount of work.
  int max_jobs = 2 * n_consumers;
  q->num_consumers = n_consumers;
  q->job_queue = calloc(sizeof(*q->job_queue), max_jobs);
  q->jobs_completed = calloc(sizeof(*q->jobs_completed), n_consumers);
  q->jobs_asked = calloc(sizeof(*q->jobs_asked), n_consumers);
  q->jobs_received = calloc(sizeof(*q->jobs_received), n_consumers);

  pthread_mutex_init(&q->lock, NULL);

  // https://stackoverflow.com/questions/16239864/non-busy-blocking-q-implementation-in-c
  sem_init(&q->empty, 1, 0);
  sem_init(&q->full, 1, max_jobs - 1);

  return q;
}

/**
 * Destroy a job queue
 */
void job_queue_destroy(struct job_queue* q) {
  free(q->jobs_received);
  free(q->jobs_asked);
  free(q->jobs_completed);
  free(q->job_queue);
  free(q);
}

/**
 * Called when there are no more jobs to add
 *
 * Waits for all jobs to complete then calls all the threads to exit
 */
void end_queue(struct job_queue* q) {
  int num_jobs = 1;
  // this is horrible but everything is mostly correct
  while (num_jobs > 0) {
    pthread_mutex_lock(&q->lock);
    num_jobs = q->queue_counter;
    pthread_mutex_unlock(&q->lock);
  }

  // let consumers know there are no more jobs
  pthread_mutex_lock(&q->lock);
  q->queue_counter = NO_MORE_JOBS;
  pthread_mutex_unlock(&q->lock);

  // add enough posts so that no threads are blocked
  for (int i = 0; i < q->num_consumers; ++i) {
    sem_post(&q->empty);
  }
}

/**
 * Consume a job from the queue
 * Blocks if there are no jobs in the queue
 *
 * Returns the total number of jobs after consuming a job
 */
int consume(struct job_queue* q, int* job) {
  int queue_num = 0;
  sem_wait(&q->empty);
  pthread_mutex_lock(&q->lock);
  if (q->queue_counter == NO_MORE_JOBS) {
    *job = NO_MORE_JOBS;
  } else {
    *job = q->job_queue[--q->queue_counter];
    queue_num = q->queue_counter;
  }
  pthread_mutex_unlock(&q->lock);
  sem_post(&q->full);
  return queue_num;
}

/**
 * Produce a job for the queue
 * Blocks if the queue is full
 *
 * Returns the total number of jobs after producing a job
 */
int produce(struct job_queue* q, int job) {
  int queue_num;
  sem_wait(&q->full);
  pthread_mutex_lock(&q->lock);
  q->job_queue[q->queue_counter++] = job;
  queue_num = q->queue_counter;
  pthread_mutex_unlock(&q->lock);
  sem_post(&q->empty);
  return queue_num;
}