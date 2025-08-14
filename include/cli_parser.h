#ifndef CLI_PARSER_H
#define CLI_PARSER_H

typedef struct {
    char src_path[256];
    char dest_path[256];
    int port;
    int mode;         
    int is_remote;    
    char remote_ip[64];
} cli_config;

cli_config parse_args(int argc, char* argv[]);

#endif
