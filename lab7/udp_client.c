#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888

int main() {
    int client_socket_fd;
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE];

    client_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket_fd == -1) {
        perror("socket create error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    while (1) {
        fgets(buffer, sizeof(buffer), stdin);

        size_t len = strlen(buffer);
        if (buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        
        if (strcmp(buffer, "x") == 0) {
            break;
        }

        ssize_t sent_bytes = sendto(client_socket_fd, buffer, strlen(buffer), 0, (struct sockaddr*) &server_addr, addr_len);
        if (sent_bytes == -1) {
            perror("error send bytes");
            close(client_socket_fd);
            exit(1);
        }

        ssize_t recv_bytes = recvfrom(client_socket_fd, buffer, sizeof(buffer), 0, NULL, NULL);
        if (recv_bytes == -1) {
            perror("error recieve bytes");
            close(client_socket_fd);
            exit(1);
        }

        printf("data from server: %s\n", buffer);
    }

    close(client_socket_fd);

    return 0;
}