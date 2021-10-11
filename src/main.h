#ifndef MAIN_H_
#define MAIN_H_

#define EVENT_STR_SIZE 10

typedef enum {
  WORK_E = 'W',
  COMPLETE_E = 'C',
  ASK_E = 'A',
  RECEIVE_E = 'R',
  END_E = 'E',
  SLEEP_E = 'S'
} Event;

typedef enum { TRANSACTION = 'T', SLEEP = 'S' } Request;

typedef struct {
  Request req;
  int arg;
} Job;

typedef struct {
  int num_jobs;
  int max_jobs;
  Job *job_queue;
} JobQueue;

#endif
