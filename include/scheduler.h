
#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "event_queue.h"
void *run_scheduler(void *arg);

typedef struct {
const char *directory;
EventQueue *queue;
int interval; 
} SchedulerArgs;
#endif
