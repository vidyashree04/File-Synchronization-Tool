#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include "scheduler.h"
#include "event_queue.h"

static void scan(const char *dir, EventQueue *queue) {
    DIR *d = opendir(dir);
    if (!d) return;

    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;

        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/%s", dir, ent->d_name);

        struct stat st;
        if (stat(path, &st) == -1) continue;

        if (S_ISREG(st.st_mode))
            enqueue_event(queue, EVENT_MODIFY, path);
        else if (S_ISDIR(st.st_mode))
            scan(path, queue);
    }

    closedir(d);
}

void* run_scheduler(void* arg) {
    SchedulerArgs *args = (SchedulerArgs*)arg;
    while (1) {
        scan(args->directory, args->queue);
        sleep(args->interval);
    }
    return NULL;
}
