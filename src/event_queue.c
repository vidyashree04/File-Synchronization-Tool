#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "event_queue.h"

void init_event_queue(EventQueue *q) {
    q->front = q->rear = NULL;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_empty, NULL);
}

void enqueue_event(EventQueue *q, EventType type, const char *filename) {
    Event *e = malloc(sizeof(Event));
    e->type = type;
    strncpy(e->filename, filename, sizeof(e->filename));
    e->next = NULL;

    pthread_mutex_lock(&q->lock);
    if (q->rear)
        q->rear->next = e;
    else
        q->front = e;
    q->rear = e;
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}

Event* dequeue_event(EventQueue *q) {
    pthread_mutex_lock(&q->lock);
    while (!q->front)
        pthread_cond_wait(&q->not_empty, &q->lock);

    Event *e = q->front;
    q->front = e->next;
    if (!q->front) q->rear = NULL;
    pthread_mutex_unlock(&q->lock);
    return e;
}

void destroy_event_queue(EventQueue *q) {
    Event *e = q->front;
    while (e) {
        Event *tmp = e;
        e = e->next;
        free(tmp);
    }
    pthread_mutex_destroy(&q->lock);
    pthread_cond_destroy(&q->not_empty);
}
