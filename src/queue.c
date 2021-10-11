#include <stdlib.h>

#include "main.h"

JobQueue* new_queue(int n_consumers) {
  JobQueue* queue = malloc(sizeof(*queue));
  queue->num_jobs = 0;
  // The queue must be able to hold 2 x #consumers amount of work.
  queue->max_jobs = 2 * n_consumers;
  queue->job_queue = malloc(sizeof(*queue->job_queue) * queue->max_jobs);

  return queue;
}

void delete_queue(JobQueue* queue) {
  free(queue->job_queue);
  free(queue);
}

Job consume(JobQueue* queue) {
  if (queue->num_jobs == 0) {
    // wait here
  }
  return queue->job_queue[--queue->num_jobs];
}

void produce(JobQueue* queue, Job job) {
  if (queue->num_jobs == queue->max_jobs) {
    // wait here
  }
  queue->job_queue[queue->num_jobs++] = job;
}