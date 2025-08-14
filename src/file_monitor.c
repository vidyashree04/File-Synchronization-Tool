#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include "event_queue.h"

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + NAME_MAX))

void start_monitoring(const char *directory, EventQueue *queue) {
    int fd = inotify_init();
    if (fd < 0) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    int wd = inotify_add_watch(fd, directory, IN_CREATE | IN_MODIFY | IN_DELETE);
    if (wd == -1) {
        fprintf(stderr, "Cannot watch '%s': %s\n", directory, strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Monitoring changes in directory: %s\n", directory);

    char buffer[BUF_LEN];
    while (1) {
        ssize_t length = read(fd, buffer, BUF_LEN);
        if (length < 0) {
            perror("read");
            break;
        }

        for (size_t i = 0; i < length;) {
            struct inotify_event *event = (struct inotify_event*)&buffer[i];
            if (event->len > 0) {
                char full[PATH_MAX];
                snprintf(full, sizeof(full), "%s/%s", directory, event->name);

                if (event->mask & IN_CREATE)
                    enqueue_event(queue, EVENT_CREATE, full);
                else if (event->mask & IN_MODIFY)
                    enqueue_event(queue, EVENT_MODIFY, full);
                else if (event->mask & IN_DELETE)
                    enqueue_event(queue, EVENT_DELETE, full);
            }
            i += EVENT_SIZE + event->len;
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);
}
