#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli_parser.h"

cli_config parse_args(int argc, char* argv[]) {
    cli_config c;

    
    strcpy(c.src_path, "test/watch_dir");
    strcpy(c.dest_path, "test/target_dir");
    strcpy(c.remote_ip, "127.0.0.1");
    c.port = 8080;
    c.mode = 0;         
    c.is_remote = 0;

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--src=", 6) == 0) {
            strncpy(c.src_path, argv[i] + 6, sizeof(c.src_path) - 1);
            c.src_path[sizeof(c.src_path) - 1] = '\0';
        } 
        else if (strncmp(argv[i], "--dest=", 7) == 0) {
            strncpy(c.dest_path, argv[i] + 7, sizeof(c.dest_path) - 1);
            c.dest_path[sizeof(c.dest_path) - 1] = '\0';
        } 
        else if (strncmp(argv[i], "--port=", 7) == 0) {
            c.port = atoi(argv[i] + 7);
        } 
        else if (strncmp(argv[i], "--mode=", 7) == 0) {
            const char* mode_val = argv[i] + 7;
            if (strcmp(mode_val, "twoway") == 0)
                c.mode = 1;
            else if (strcmp(mode_val, "oneway") == 0)
                c.mode = 0;
            else {
                fprintf(stderr, " Invalid mode: %s (use 'oneway' or 'twoway')\n", mode_val);
                exit(EXIT_FAILURE);
            }
        } 
        else if (strcmp(argv[i], "--remote") == 0) {
            c.is_remote = 1;
        } 
        else if (strncmp(argv[i], "--ip=", 5) == 0) {
            strncpy(c.remote_ip, argv[i] + 5, sizeof(c.remote_ip) - 1);
            c.remote_ip[sizeof(c.remote_ip) - 1] = '\0';
        } 
        else {
            fprintf(stderr, " Unknown argument: %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }

    return c;
}
