#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include "sync_manager.h"
#include "metadata_handler.h"
#include "network_transfer.h"
#include "cli_parser.h"

extern cli_config config;

#define MAX_TRACKED 100

typedef struct {
    char path[512];
    time_t last_mtime;
    time_t last_synced;
} SyncEntry;

static SyncEntry tracked[MAX_TRACKED];
static int tracked_count = 0;

int should_sync(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return 0;

    time_t now = time(NULL);
    for (int i = 0; i < tracked_count; ++i) {
        if (strcmp(tracked[i].path, path) == 0) {
            if (st.st_mtime == tracked[i].last_mtime && now - tracked[i].last_synced < 2)
                return 0;

            tracked[i].last_mtime = st.st_mtime;
            tracked[i].last_synced = now;
            return 1;
        }
    }

    if (tracked_count < MAX_TRACKED) {
        strncpy(tracked[tracked_count].path, path, sizeof(tracked[tracked_count].path));
        tracked[tracked_count].last_mtime = st.st_mtime;
        tracked[tracked_count].last_synced = now;
        tracked_count++;
    }

    return 1;
}

void sync_create_or_update(const char *src_path, const char *dst_path) {
    if (!should_sync(src_path)) return;

    static char last_synced_path[512] = "";
    static time_t last_print_time = 0;

    time_t now = time(NULL);
    if (strcmp(last_synced_path, src_path) != 0 || now - last_print_time > 2) {
        
        
        strncpy(last_synced_path, src_path, sizeof(last_synced_path));
        last_print_time = now;
    }

    sleep(1); 

    struct stat src_stat, dst_stat;
    if (stat(src_path, &src_stat) != 0) return;

    int copy = 1;
    if (stat(dst_path, &dst_stat) == 0) {
        if (src_stat.st_mtime <= dst_stat.st_mtime)
            copy = 0;
    }

    if (!copy) return;

    if (config.is_remote) {
        const char *filename = strrchr(src_path, '/');
        filename = filename ? filename + 1 : src_path;
        start_client(config.remote_ip, config.port, src_path, filename);
        return;
    }

    int src_fd = open(src_path, O_RDONLY);
    if (src_fd < 0) return;

    char dst_dir[512];
    strcpy(dst_dir, dst_path);
    char *slash = strrchr(dst_dir, '/');
    if (slash) {
        *slash = '\0';
        char cmd[600];
        snprintf(cmd, sizeof(cmd), "mkdir -p \"%s\"", dst_dir);
        system(cmd);
    }

    int dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, src_stat.st_mode);
    if (dst_fd < 0) {
        close(src_fd);
        return;
    }

    char buf[4096];
    ssize_t bytes;
    while ((bytes = read(src_fd, buf, sizeof(buf))) > 0)
        write(dst_fd, buf, bytes);

    close(src_fd);
    close(dst_fd);

    preserve_metadata(src_path, dst_path);
    printf(" Synced: %s → %s\n", src_path, dst_path);
}

void sync_delete(const char *dst_path) {
    printf("Deleting %s\n", dst_path);
    if (remove(dst_path) == 0)
        printf(" Deleted: %s\n", dst_path);
}
