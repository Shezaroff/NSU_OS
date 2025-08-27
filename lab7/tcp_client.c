#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888

int main() {
    int client_socket_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket_fd == -1) {
        perror("error create socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    if (connect(client_socket_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
        perror("error socket connect");
        close(client_socket_fd);
        exit(EXIT_FAILURE);
    }
    printf("connected to server\n");

    while (1) {
        fgets(buffer, sizeof(buffer), stdin);

        size_t len = strlen(buffer);
        if (buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        
        if (strcmp(buffer, "x") == 0) {
            break;
        }

        if (write(client_socket_fd, buffer, len) == -1) {
            perror("error write");
            close(client_socket_fd);
            exit(EXIT_FAILURE);
        }

        bytes_read = read(client_socket_fd, buffer, len);
        if (bytes_read == -1) {
            perror("error read");
            close(client_socket_fd);
            exit(EXIT_FAILURE);
        }

        printf("data from server: %s\n", buffer);
    }

    close(client_socket_fd);
    return 0;
}