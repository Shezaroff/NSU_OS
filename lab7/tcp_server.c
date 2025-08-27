#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <signal.h>

#define BUFFER_SIZE 1024

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888

struct Client {
    int socket_fd;
    struct sockaddr_in address;
};

void handle_client(struct Client* client) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    printf("new connection from: %s:%d  %d\n", inet_ntoa(client->address.sin_addr), ntohs(client->address.sin_port), getpid());

    while (1) {
        bytes_read = read(client->socket_fd, buffer, sizeof(buffer));
        if (bytes_read == 0) {
            printf("client disconnected: %s:%d\n", inet_ntoa(client->address.sin_addr), ntohs(client->address.sin_port));
            close(client->socket_fd);
            exit(EXIT_SUCCESS);
        } else {
            if (bytes_read < 0) {
                perror("read error");
                exit(EXIT_FAILURE);
            }
        }

        if (write(client->socket_fd, buffer, bytes_read) != bytes_read) {
            perror("send error");
            close(client->socket_fd);
            exit(EXIT_FAILURE);
        }
    }
}

int main() {
    int server_socket_fd, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd == -1) {
        perror("error create socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(server_socket_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
        perror("error socket bind");
        close(server_socket_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket_fd, 10) == -1) {
        perror("error listen socket");
        close(server_socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server was started...\n");

    signal(SIGCHLD, SIG_IGN);

    while (1) {
        client_socket = accept(server_socket_fd, (struct sockaddr*) &client_addr, &addr_len);
        if (client_socket == -1) {
            perror("error accept socket");
            close(server_socket_fd);
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork error");
            close(client_socket);
            close(server_socket_fd);
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            close(server_socket_fd);
            struct Client client;
            client.socket_fd = client_socket;
            client.address = client_addr;
            handle_client(&client);
        } else {
            close(client_socket);
        }
    }
    close(server_socket_fd);
    return EXIT_SUCCESS;
}