
#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H
#include <pthread.h>

typedef enum {
EVENT_CREATE,
EVENT_MODIFY,

EVENT_DELETE
} EventType;

typedef struct Event {
EventType type;
char filename[256];
struct Event *next;
} Event;
typedef struct {
Event *front;
Event *rear;
pthread_mutex_t lock;
pthread_cond_t not_empty;
} EventQueue;

void init_event_queue(EventQueue *queue);
void enqueue_event(EventQueue *queue, EventType type, const char *filename);
Event* dequeue_event(EventQueue *queue);
void destroy_event_queue(EventQueue *queue);
#endif
