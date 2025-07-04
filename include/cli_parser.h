#ifndef CLI_PARSER_H
#define CLI_PARSER_H

typedef struct {
    char src_path[256];   // Source directory
    char dest_path[256];  // Destination directory
    int port;             // TCP port
    int mode;             // 0 = oneway, 1 = twoway (future support)
} cli_config;

cli_config parse_args(int argc, char* argv[]);

#endif
