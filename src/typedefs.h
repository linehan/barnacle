/* This data structure uses the same technique as the kernel's linked lists
 * to bootstrap its way into functions where only one element was passed. This
 * technique uses the macro container_of defined in container_of.h, a dep of
 * the list.h used to handle linked lists. The typedef 'OO' is meant to look
 * like two eyeballs, not any kind of references to object orientation. */
typedef struct watcher_timer {
        struct ev_loop *loop;
        ev_timer w; /* watcher */
        sem_t *sem;
        void *data;
} OO_time;

typedef struct watcher_io {
        struct ev_loop *loop;
        ev_io w; /* watcher */
        sem_t *sem;
        void *data;
} OO_io;
