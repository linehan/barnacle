/*
 * Declarations for the clients of a thread pool.
 */

#include <pthread.h>
#include <stdint.h>

/*
 * The thr_pool_t type is opaque to the client. It is created by 
 * thr_pool_create() and must be passed unmodified to the remainder 
 * of the interfaces.
 */
typedef struct thr_pool thr_pool_t;
typedef uint32_t uint_t;

/*
 * thr_pool_create -- create a thread pool.
 *
 * @min_threads: the minimum number of free threads kept in the pool
 * @max_threads: the maximum number of threads that can be in the pool
 * @linger: number of seconds idle worker threads (> min_threads) linger 
 * before exiting.
 * @attr: attributes of all worker threads (can be NULL); 
 *
 * On error, thr_pool_create() returns NULL with errno set to the error code.
 */
extern    
thr_pool_t *thr_pool_create(uint32_t min_threads, uint32_t max_threads,
                            uint32_t linger,      pthread_attr_t *attr);


/*
 * thr_pool_queue -- enqueue a work request to the thread pool job queue.
 *
 * If there are idle worker threads, awaken one to perform the job, else if 
 * the maximum number of workers has not been reached, create a new worker 
 * thread to perform the job, or else just return after adding the job to 
 * the queue; an existing worker thread will perform the job when it 
 * finishes the job it is currently performing.
 *
 * The job is performed as if a new detached thread were created for it:
 *    pthread_create(NULL, attr, void *(*func)(void *), void *arg);
 *
 * On error, thr_pool_queue() returns -1 with errno set to the error code.
 */
extern    
int thr_pool_queue(thr_pool_t *pool, void *(*func)(void *), void *arg);

/*
 * thr_pool_wait -- wait for all queued jobs to complete.
 */
extern    
void thr_pool_wait(thr_pool_t *pool);

/*
 * thr_pool_destroy -- cancel all queued jobs and destroy the pool.
 */
extern
void thr_pool_destroy(thr_pool_t *pool);
