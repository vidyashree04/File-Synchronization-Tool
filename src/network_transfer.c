#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include "network_transfer.h"

#define BUFFER_SIZE 1024
#define PATH_MAX_LEN 2048  // Support long file paths

void create_directories(const char* filepath) {
    char command[PATH_MAX_LEN];
    snprintf(command, sizeof(command), "mkdir -p \"$(dirname '%s')\"", filepath);
    system(command);
}

// ==================== SERVER ====================
int start_server(int port, const char* base_dest_dir) {
    int server_fd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        return -1;
    }

    listen(server_fd, 10);  // Handle up to 10 simultaneous clients (adjustable)
    printf("🟢 Server listening on port %d...\n", port);

    while (1) {
        int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("accept");
            continue;
        }

        // === Step 1: Receive file name ===
        char relative_path[PATH_MAX_LEN];
        int path_len = recv(new_socket, relative_path, sizeof(relative_path), 0);
        if (path_len <= 0) {
            perror("Failed to receive file path");
            close(new_socket);
            continue;
        }

        // === Step 2: Build full path ===
        char full_dest_path[PATH_MAX_LEN];
        int written = snprintf(full_dest_path, sizeof(full_dest_path), "%s/%s", base_dest_dir, relative_path);
        if (written >= sizeof(full_dest_path)) {
            fprintf(stderr, "⚠️ Error: path too long — skipping file\n");
            close(new_socket);
            continue;
        }

        create_directories(full_dest_path);

        // === Step 3: Open file to write ===
        int fd = open(full_dest_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd < 0) {
            perror("file open");
            close(new_socket);
            continue;
        }

        // === Step 4: Receive file content ===
        int bytes_received;
        while ((bytes_received = read(new_socket, buffer, BUFFER_SIZE)) > 0) {
            write(fd, buffer, bytes_received);
        }

        close(fd);
        close(new_socket);

        printf("✅ File received and saved to %s\n", full_dest_path);
    }

    close(server_fd);
    return 0;
}

// ==================== CLIENT ====================
int start_client(const char* ip, int port, const char* src_file, const char* relative_path) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    int fd = open(src_file, O_RDONLY);
    if (fd < 0) {
        perror("file open");
        return -1;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation error");
        close(fd);
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        printf("Invalid address\n");
        close(fd);
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        close(fd);
        return -1;
    }

    // === Step 1: Send relative path first ===
    send(sock, relative_path, strlen(relative_path) + 1, 0);

    // === Step 2: Send file content ===
    int bytes_read;
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        send(sock, buffer, bytes_read, 0);
    }

    close(fd);
    close(sock);

    printf("✅ File sent: %s (as %s)\n", src_file, relative_path);
    return 0;
}
