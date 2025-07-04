#ifndef NETWORK_TRANSFER_H
#define NETWORK_TRANSFER_H

int start_server(int port, const char* base_dest_dir);
int start_client(const char* ip, int port, const char* src_file, const char* relative_path);

#endif
