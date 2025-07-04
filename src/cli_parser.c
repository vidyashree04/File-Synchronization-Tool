#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli_parser.h"

cli_config parse_args(int argc, char* argv[]) {
    cli_config config;

    strcpy(config.src_path, "test/watch_dir");
    strcpy(config.dest_path, "test/target_dir");
    config.port = 8080;
    config.mode = 0;

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--src=", 6) == 0) {
            strncpy(config.src_path, argv[i] + 6, sizeof(config.src_path) - 1);
        }
        else if (strncmp(argv[i], "--dest=", 7) == 0) {
            strncpy(config.dest_path, argv[i] + 7, sizeof(config.dest_path) - 1);
        }
        else if (strncmp(argv[i], "--port=", 7) == 0) {
            config.port = atoi(argv[i] + 7);
        }
        else if (strncmp(argv[i], "--mode=", 7) == 0) {
            if (strcmp(argv[i] + 7, "oneway") == 0) config.mode = 0;
            else if (strcmp(argv[i] + 7, "twoway") == 0) config.mode = 1;
            else {
                fprintf(stderr, "Invalid mode: %s\n", argv[i] + 7);
                exit(EXIT_FAILURE);
            }
        }
    }

    return config;
}
