#ifndef LOGGER_H_
#define LOGGER_H_

#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>

#include "job_queue.h"

/**
 * Struct for multi-threaded logging
 */
struct logger {
  FILE* outfile_fp;      // pointer to output file
  struct timeval start;  // timeval struct to store start time for logging
  pthread_mutex_t outfile_lock;  // mutex lock for output file
};

/**
 * Initialize file logger given a filename
 */
struct logger* logger_init(char const* filename);

/**
 * Closes file and destroys logger
 */
void logger_destroy(struct logger* l);

/**
 * Logs when a consumer with ID of `id` asks for a job
 */
void log_ask(struct logger* l, int id);

/**
 * Logs when the producer with ID of 0 sleeps
 * or when a job is completed by a consumer with ID >= 1
 */
void log_with_arg(struct logger* l, int id, int arg);

/**
 * Logs when the producer with ID of 0 adds work to the queue
 * or when a consumer with ID >= 1 removes work from the queue
 */
void log_with_q_and_arg(struct logger* l, int id, int q, int arg);

/**
 * Prints the summary of job execution after all work is completed
 */
void print_summary(struct logger* l, struct job_queue* q);

#endif
