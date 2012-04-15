/*
 * Thread pool implementation.
 * See <thr_pool.h> for interface declarations.
 */

/*#ifndef(_REENTRANT)*/
/*#define _REENTRANT*/
/*#endif*/

#include "thr_pool.h"
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

/*
 * FIFO queued job
 */
typedef struct job job_t;
struct job {
    job_t *job_next;             /* linked list of jobs */
    void  *(*job_func)(void *);  /* function to call */
    void  *job_arg;              /* its argument */
};

/*
 * List of active worker threads, linked through their stacks.
 */
typedef struct active active_t;
struct active {
    active_t  *active_next;   /* linked list of threads */
    pthread_t  active_tid;    /* active thread id */
};

/*
 * The thread pool, opaque to the clients.
 */
struct thr_pool {
    thr_pool_t      *forw;     /* circular linked list */
    thr_pool_t      *back;     /* of all thread pools */
    pthread_mutex_t  mutex;    /* protects the pool data */
    pthread_cond_t   busycv;   /* synchronization in pool_queue */
    pthread_cond_t   workcv;   /* synchronization with workers */
    pthread_cond_t   waitcv;   /* synchronization in pool_wait() */
    active_t        *active;   /* list of threads performing work */
    job_t           *head;     /* head of FIFO job queue */
    job_t           *tail;     /* tail of FIFO job queue */
    pthread_attr_t   attr;     /* attributes of the workers */
    int              flags;    /* see below */
    uint_t           linger;   /* seconds before idle workers exit */
    int              minimum;  /* minimum number of worker threads */
    int              maximum;  /* maximum number of worker threads */
    int              nthreads; /* current number of worker threads */
    int              idle;     /* number of idle workers */
};

/* flags */
#define POOL_WAIT    0x01  /* waiting in thr_pool_wait() */
#define POOL_DESTROY 0x02  /* pool is being destroyed */

/* the list of all created and not yet destroyed thread pools */
static thr_pool_t *thr_pools = NULL;

/* protects thr_pools */
static pthread_mutex_t thr_pool_lock = PTHREAD_MUTEX_INITIALIZER;

/* set of all signals */
static sigset_t fillset;

static void *worker_thread(void *);


/*
 * Creates a new worker thread in pool
 */
static int
create_worker(thr_pool_t *pool)
{
        sigset_t oset;
        int error;

        (void) pthread_sigmask(SIG_SETMASK, &fillset, &oset);
        error = pthread_create(NULL, &pool->attr, worker_thread, pool);
        (void) pthread_sigmask(SIG_SETMASK, &oset, NULL);
        return (error);
}


/*
 * Worker thread is terminating.  Possible reasons:
 * - excess idle thread is terminating because there is no work.
 * - thread was cancelled (pool is being destroyed).
 * - the job function called pthread_exit().
 * In the last case, create another worker thread if necessary, to keep 
 * the pool populated.
 */
static void
worker_cleanup(thr_pool_t *pool)
{
        /* One less thread */
        --pool->nthreads;
        if (pool->flags & POOL_DESTROY) {
                if (pool->nthreads == 0)
                        (void) pthread_cond_broadcast(&pool->busycv);
        /* Keep alive */
        } else if (pool->head != NULL 
               &&  pool->nthreads < pool->maximum 
               &&  create_worker(pool) == 0) 
                {
                        pool->nthreads++;
                }
        (void) pthread_mutex_unlock(&pool->mutex);
}


static void
notify_waiters(thr_pool_t *pool)
{
        if (pool->head == NULL && pool->active == NULL) {
                pool->flags &= ~POOL_WAIT;
                (void) pthread_cond_broadcast(&pool->waitcv);
    }
}

/*
 * Called by a worker thread on return from a job.
 */
static void
job_cleanup(thr_pool_t *pool)
{
        pthread_t my_tid = pthread_self();
        active_t *activep;
        active_t **activepp;

        (void) pthread_mutex_lock(&pool->mutex);
        for (activepp = &pool->active;
             (activep = *activepp) != NULL;
             activepp = &activep->active_next) 
        {
                if (activep->active_tid == my_tid) {
                        *activepp = activep->active_next;
                        break;
                }
        }
        if (pool->flags & POOL_WAIT)
                notify_waiters(pool);
}


static void *
worker_thread(void *arg)
{
        thr_pool_t *pool = (thr_pool_t *)arg;
        int timedout;
        job_t *job;
        void *(*func)(void *);
        active_t active;
        struct timespec ts;

        /*
         * This is the worker's main loop.  It will only be left
         * if a timeout occurs or if the pool is being destroyed.
         */
        pthread_mutex_lock(&pool->mutex);
        pthread_cleanup_push(worker_cleanup, pool);
        active.active_tid = pthread_self();
        for (;;) {
                /*
                 * We don't know what this thread was doing during
                 * its last job, so we reset its signal mask and
                 * cancellation state back to the initial values.
                 */
                pthread_sigmask(SIG_SETMASK, &fillset, NULL);
                pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
                pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

                timedout = 0;
                pool->idle++;

                if (pool->flags & POOL_WAIT)
                        notify_waiters(pool);
                while (pool->head == NULL 
                   && !(pool->flags & POOL_DESTROY)) 
                {
                        if (pool->nthreads <= pool->minimum) {
                                pthread_cond_wait(&pool->workcv,
                                                  &pool->mutex);
                        } else {
                                clock_gettime(CLOCK_REALTIME, &ts);
                                ts.tv_sec += pool->linger;
                                if (pool->linger == 0 
                                ||  pthread_cond_timedwait(&pool->workcv,
                                                           &pool->mutex, 
                                                           &ts) == ETIMEDOUT) 
                                {
                                        timedout = 1;
                                        break;
                                }
                        }
                }

                pool->idle--;

                if (pool->flags & POOL_DESTROY)
                        break;

                if ((job = pool->head) != NULL) {
                        timedout        = 0;
                        func            = job->job_func;
                        arg             = job->job_arg;
                        pool->head = job->job_next;

                        if (job == pool->tail)
                                pool->tail = NULL;

                        active.active_next = pool->active;
                        pool->active  = &active;

                        pthread_mutex_unlock(&pool->mutex);
                        pthread_cleanup_push(job_cleanup, pool);

                        free(job);
                        /*
                         * Call the specified job function.
                         */
                        func(arg);
                        /*
                         * If the job function calls pthread_exit(), the thread
                         * calls job_cleanup(pool) and worker_cleanup(pool);
                         * the integrity of the pool is thereby maintained.
                         */
                        pthread_cleanup_pop(1);    /* job_cleanup(pool) */
                }
                if (timedout && pool->nthreads > pool->minimum) {
                       /*
                        * We timed out and there is no work to be done
                        * and the number of workers exceeds the minimum.
                        * Exit now to reduce the size of the pool.
                        */
                        break;
                }
        }
        pthread_cleanup_pop(1);    /* worker_cleanup(pool) */
        return (NULL);
}



static void
clone_attributes(pthread_attr_t *new_attr, pthread_attr_t *old_attr)
{
        struct sched_param param;
        void *addr;
        size_t size;
        int value;

        (void) pthread_attr_init(new_attr);

        if (old_attr != NULL) {
                (void) pthread_attr_getstack(old_attr, &addr, &size);
                /* don't allow a non-NULL thread stack address */
                (void) pthread_attr_setstack(new_attr, NULL, size);

                (void) pthread_attr_getscope(old_attr, &value);
                (void) pthread_attr_setscope(new_attr, value);

                (void) pthread_attr_getinheritsched(old_attr, &value);
                (void) pthread_attr_setinheritsched(new_attr, value);

                (void) pthread_attr_getschedpolicy(old_attr, &value);
                (void) pthread_attr_setschedpolicy(new_attr, value);

                (void) pthread_attr_getschedparam(old_attr, &param);
                (void) pthread_attr_setschedparam(new_attr, &param);

                (void) pthread_attr_getguardsize(old_attr, &size);
                (void) pthread_attr_setguardsize(new_attr, size);
        }

        /* make all pool threads be detached threads */
        (void) pthread_attr_setdetachstate(new_attr, PTHREAD_CREATE_DETACHED);
}



thr_pool_t *
thr_pool_create(uint_t min_threads, uint_t max_threads, uint_t linger,
                pthread_attr_t *attr)
{
        thr_pool_t *pool;

        (void) sigfillset(&fillset);

        if (min_threads > max_threads || max_threads < 1) {
                errno = EINVAL;
                return (NULL);
        }

        if ((pool = malloc(sizeof (*pool))) == NULL) {
                errno = ENOMEM;
                return (NULL);
        }

        (void) pthread_mutex_init(&pool->mutex, NULL);
        (void) pthread_cond_init(&pool->busycv, NULL);
        (void) pthread_cond_init(&pool->workcv, NULL);
        (void) pthread_cond_init(&pool->waitcv, NULL);

        pool->active   = NULL;
        pool->head     = NULL;
        pool->tail     = NULL;
        pool->flags    = 0;
        pool->linger   = linger;
        pool->minimum  = min_threads;
        pool->maximum  = max_threads;
        pool->nthreads = 0;
        pool->idle     = 0;

        /*
         * We cannot just copy the attribute pointer.
         * We need to initialize a new pthread_attr_t structure using
         * the values from the caller-supplied attribute structure.
         * If the attribute pointer is NULL, we need to initialize
         * the new pthread_attr_t structure with default values.
         */
        clone_attributes(&pool->attr, attr);

        /* insert into the global list of all thread pools */
        (void) pthread_mutex_lock(&thr_pool_lock);
        if (thr_pools == NULL) {
                pool->forw = pool;
                pool->back = pool;
                thr_pools = pool;
        } else {
                thr_pools->back->forw = pool;
                pool->forw                 = thr_pools;
                pool->back                 = thr_pools->back;
                thr_pools->back            = pool;
        }
        (void) pthread_mutex_unlock(&thr_pool_lock);

        return (pool);
}



int
thr_pool_queue(thr_pool_t *pool, void *(*func)(void *), void *arg)
{
        job_t *job;

        if ((job = malloc(sizeof (*job))) == NULL) {
                errno = ENOMEM;
                return (-1);
        }

        job->job_next = NULL;
        job->job_func = func;
        job->job_arg = arg;

        (void) pthread_mutex_lock(&pool->mutex);

        if (pool->head == NULL)
                pool->head = job;
        else
                pool->tail->job_next = job;

        pool->tail = job;

        if (pool->idle > 0)
                (void) pthread_cond_signal(&pool->workcv);
        else if (pool->nthreads < pool->maximum &&
                create_worker(pool) == 0)
                pool->nthreads++;

        (void) pthread_mutex_unlock(&pool->mutex);

        return (0);
}



void
thr_pool_wait(thr_pool_t *pool)
{
        (void) pthread_mutex_lock(&pool->mutex);
        pthread_cleanup_push(pthread_mutex_unlock, &pool->mutex);
        while (pool->head != NULL || pool->active != NULL) {
                pool->flags |= POOL_WAIT;
                (void) pthread_cond_wait(&pool->waitcv, &pool->mutex);
        }
        pthread_cleanup_pop(1);    /* pthread_mutex_unlock(&pool->mutex); */
}



void
thr_pool_destroy(thr_pool_t *pool)
{
        active_t *activep;
        job_t *job;

        (void) pthread_mutex_lock(&pool->mutex);
        pthread_cleanup_push(pthread_mutex_unlock, &pool->mutex);

        /* mark the pool as being destroyed; wakeup idle workers */
        pool->flags |= POOL_DESTROY;
        (void) pthread_cond_broadcast(&pool->workcv);

        /* cancel all active workers */
        for (activep = pool->active;
             activep != NULL;
             activep = activep->active_next)
                (void) pthread_cancel(activep->active_tid);

                /* wait for all active workers to finish */
                while (pool->active != NULL) {
                        pool->flags |= POOL_WAIT;
                (void) pthread_cond_wait(&pool->waitcv, &pool->mutex);
        }

        /* the last worker to terminate will wake us up */
        while (pool->nthreads != 0)
                (void) pthread_cond_wait(&pool->busycv, &pool->mutex);

        pthread_cleanup_pop(1);    /* pthread_mutex_unlock(&pool->mutex); */

        /*
         * Unlink the pool from the global list of all pools.
         */
        (void) pthread_mutex_lock(&thr_pool_lock);
        if (thr_pools == pool)
                thr_pools = pool->forw;
        if (thr_pools == pool)
                thr_pools = NULL;
        else {
                pool->back->forw = pool->forw;
                pool->forw->back = pool->back;
        }
        (void) pthread_mutex_unlock(&thr_pool_lock);

        /*
         * There should be no pending jobs, but just in case...
         */
        for (job = pool->head; job != NULL; job = pool->head) {
                pool->head = job->job_next;
                free(job);
        }
        (void) pthread_attr_destroy(&pool->attr);
        free(pool);
}
