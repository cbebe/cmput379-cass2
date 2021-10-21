#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

#include "main.h"

JobQueue* new_queue(int n_consumers) {
  JobQueue* queue = malloc(sizeof(*queue));
  queue->queue_counter = 0;
  // The queue must be able to hold 2 x #consumers amount of work.
  int max_jobs = 2 * n_consumers;
  queue->num_consumers = n_consumers;
  queue->job_queue = calloc(sizeof(*queue->job_queue), max_jobs);
  queue->jobs_completed = calloc(sizeof(*queue->jobs_completed), n_consumers);

  pthread_mutex_init(&queue->lock, NULL);

  // https://stackoverflow.com/questions/16239864/non-busy-blocking-queue-implementation-in-c
  sem_init(&queue->full, 1, 0);
  sem_init(&queue->empty, 1, max_jobs - 1);

  return queue;
}

void delete_queue(JobQueue* queue) {
  free(queue->job_queue);
  free(queue);
}

void end_queue(JobQueue* queue) {
  int num_jobs;
  pthread_mutex_lock(&queue->lock);
  num_jobs = queue->queue_counter;
  pthread_mutex_unlock(&queue->lock);
  for (int i = 0; i < num_jobs; ++i) {
    sem_wait(&queue->empty);
  }
  pthread_mutex_lock(&queue->lock);
  queue->queue_counter = QUEUE_END;
  pthread_mutex_unlock(&queue->lock);
  for (int i = 0; i < num_jobs; ++i) {
    sem_post(&queue->full);
  }
}

int consume(JobQueue* queue, int* queue_num) {
  int job;
  sem_wait(&queue->full);
  pthread_mutex_lock(&queue->lock);
  if (queue->queue_counter < 0) {
    job = QUEUE_END;
  } else {
    job = queue->job_queue[--queue->queue_counter];
    *queue_num = queue->queue_counter;
  }
  pthread_mutex_unlock(&queue->lock);
  sem_post(&queue->empty);
  return job;
}

void produce(JobQueue* queue, int job) {
  sem_wait(&queue->empty);
  pthread_mutex_lock(&queue->lock);
  queue->job_queue[queue->queue_counter++] = job;
  pthread_mutex_unlock(&queue->lock);
  sem_post(&queue->full);
}