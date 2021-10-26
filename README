# CMPUT 379 Assignment 2 - Producer-Consumer

Charles Ancheta - cancheta

## Compilation

Run `make` to compile `prodcon` (with `-O` flag by default).
To compile for debugging, run `make debug`.
To compile with no optimizations, run `make noopt`.

## Files

### `tands.c`

Provided file containing functions `Trans(int)` and `Sleep(int)`.

- `void Trans(int n)` - Simulate a transaction with the parameter n (n > 0) determining the duration of the transaction.

- `void Sleep(int n)` - Simulate waiting for new input by sleeping for n \* 10 milliseconds

### `job_queue.c`

Contains functions for a synchronized job queue. The job queue contains the arguments for transactions (Trans(n)) that a producer adds to the queue and consumers remove from.

- `struct job_queue* job_queue_init(int n_consumers)` - Creates a queue of length `n_consumers` and allocates memory for various arrays

- `void job_queue_destroy(struct job_queue* q)` - Frees up memory allocated for the job queue

- `void end_queue(struct job_queue* q)` - Called by the producer when there is no more work to do. Waits for all jobs to finish and signals all consumers to exit.

- `int consume(struct job_queue* q, int* job)` - Removes a job from the queue. Called by the consumer.

- `int produce(struct job_queue* q, int job)` - Adds a job to the queue. Called by the producer.

### `logger.c`

Contains functions for logging to an output file.

- `struct logger* logger_init(char const* filename)` - Initialize file logger given a filename

- `void logger_destroy(struct logger* l)` - Destroys logger

- `double get_duration(struct timeval s, struct timeval e)` - Get duration between two timevals

- `void log_to_file(struct logger* l, int id, char* str)` - Log to file atomically

- `void log_ask(struct logger* l, int id)` - Log "Ask" event

- `void log_with_arg(struct logger* l, int id, int arg)` - Log "Complete" or "Sleep" event

- `void log_with_q_and_arg(struct logger* l, int id, int q, int arg)` - Log "Work" or "Receive" event

- `void print_summary(struct logger* l, struct job_queue* q)` - Print work summary

### `main.c`

Contains the main program and thread function

- `void parse_args(int* nthreads, char* filename, int argc, char const* argv[])` - Parse arguments from command line

- `void* consoomer_func(void* consumer_id)` - Consumer thread function. Removes work from the queue and executes `Trans`

- `void create_thread(pthread_t* tid, int consumer_id)` - Creates a thread running `consoomer_func`

- `void producer_func()` - Producer (main) thread function. Adds work to the queue or calls `Sleep`

- `int main(int argc, char const* argv[])` - Initializes and destroys objects and threads.

## Assumptions

The program assumes that a reasonable number of threads are created. Because this implementation uses semaphores, the maximum number of threads it can spawn is defined by SEM_VALUE_MAX.

## Solution Approach

Most of the implementation was straightforward, adding `pthread_mutex_lock`s in critical sections like the file pointer and the queue counter.  
The more complicated part was preventing the producer from adding to a full queue and the consumer from removing from an empty queue. I solved this part by initializing two semaphores, `empty`, and `full`, with `0` and `QUEUE_LENGTH - 1`, respectively.  
The produce function calls `sem_wait` on `full` before adding to the queue until it reaches zero, signifying that the queue is full. The consumers then call `sem_post` on `full` when they're done consuming to signal the producer that the queue can be added to again.  
Similarly, the consume function calls `sem_wait` on `empty` before consuming from the queue. The producer would then call `sem_post` on `empty` to signal the consumers that they can remove from the queue.

Another hurdle I came across was ending the consumer threads when the producer stops getting input. I wanted to signal the threads individually that there are no more jobs, but couldn't find a way to do that without modifying the queue (which was not safe to do if there are still unfinished jobs).  
What I ended up doing is simply wait for all remaining work to finish and set the queue counter to `-1`. Then, do enough `sem_post` on `empty` so that all consumer threads can ask for jobs one more time, check that the queue counter is `-1`, and return.
