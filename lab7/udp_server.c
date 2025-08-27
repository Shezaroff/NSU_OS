#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888

int main() {
    int server_socket_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE];

    server_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket_fd == -1) {
        perror("socket create error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(server_socket_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
        perror("error socket bind");
        close(server_socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("UDP was started on port %d\n", SERVER_PORT);

    while (1) {
        ssize_t recv_bytes = recvfrom(server_socket_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*) &client_addr, &addr_len);
        if (recv_bytes == -1) {
            perror("error recieve bytes");
            close(server_socket_fd);
            exit(EXIT_FAILURE);
        }
        buffer[recv_bytes] = '\0';

        printf("data from client %s:%d  %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);

        ssize_t sent_bytes = sendto(server_socket_fd, buffer, recv_bytes, 0, (struct sockaddr*) &client_addr, addr_len);
        if (sent_bytes == -1) {
            perror("error send bytes");
            close(server_socket_fd);
            exit(EXIT_FAILURE);
        }
    }

    close(server_socket_fd);

    return 0;
}