#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     
#include <pthread.h>
#include <sys/types.h>  
#include <sys/wait.h>

#include "cli_parser.h"
#include "event_queue.h"
#include "file_monitor.h"
#include "scheduler.h"
#include "sync_manager.h"
#include "network_transfer.h"

EventQueue queue;
cli_config config;

void* monitor_src_thread(void* arg) {
    printf(" Monitoring changes in: %s\n", config.src_path);
    start_monitoring(config.src_path, &queue);
    return NULL;
}

void* monitor_dest_thread(void* arg) {
    printf("Also monitoring changes in: %s\n", config.dest_path);
    start_monitoring(config.dest_path, &queue);
    return NULL;
}

void* event_consumer_thread(void* arg) {
    while (1) {
        Event* event = dequeue_event(&queue);
        if (!event) continue;

        char src_path[512], dest_path[512];
        const char* filename = strrchr(event->filename, '/');
        filename = filename ? filename + 1 : event->filename;

        snprintf(src_path, sizeof(src_path), "%s", event->filename);

        
        if (strncmp(event->filename, config.src_path, strlen(config.src_path)) == 0)
            snprintf(dest_path, sizeof(dest_path), "%s/%s", config.dest_path, filename);
        else
            snprintf(dest_path, sizeof(dest_path), "%s/%s", config.src_path, filename);

        if (event->type == EVENT_CREATE || event->type == EVENT_MODIFY)
            sync_create_or_update(src_path, dest_path);
        else if (event->type == EVENT_DELETE)
            sync_delete(dest_path);

        free(event);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    config = parse_args(argc, argv);
    printf("Parsed mode = %d (0 = oneway, 1 = twoway)\n", config.mode);

    init_event_queue(&queue);

    
    if (config.is_remote) {
        pid_t pid = fork();
        if (pid < 0) {
            perror(" fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            start_server(config.port, config.dest_path);
            exit(0);
        }
        sleep(1);  
    }

    pthread_t mon_src_tid, mon_dest_tid, con_tid, sched_tid;

    pthread_create(&mon_src_tid, NULL, monitor_src_thread, NULL);
    if (config.mode == 1) {
        pthread_create(&mon_dest_tid, NULL, monitor_dest_thread, NULL);
    }

    pthread_create(&con_tid, NULL, event_consumer_thread, NULL);

    SchedulerArgs args = { config.src_path, &queue, 5 };
    pthread_create(&sched_tid, NULL, run_scheduler, &args);

    pthread_join(mon_src_tid, NULL);
    if (config.mode == 1) {
        pthread_join(mon_dest_tid, NULL);
    }
    pthread_join(con_tid, NULL);
    pthread_join(sched_tid, NULL);

    destroy_event_queue(&queue);
    return 0;
}
