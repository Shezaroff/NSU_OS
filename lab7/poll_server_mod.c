#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <errno.h>

#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888
#define MAX_CLIENTS 20

typedef struct Client{
    int fd;             
    char buffer[BUFFER_SIZE]; 
    ssize_t bytes_read;    
    ssize_t bytes_written; 
} Client;

int main() {
    int server_socket_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    struct pollfd fds[MAX_CLIENTS + 1]; 
    Client clients[MAX_CLIENTS]; 
    int nfds = MAX_CLIENTS; 
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].fd = -1;
        clients[i].bytes_read = 0;
        clients[i].bytes_written = 0;
        fds[i + 1].fd = -1;
    }

    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd == -1) {
        perror("error create socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    int opt = 1;
    if (setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        close(server_socket_fd);
        exit(EXIT_FAILURE);
    }

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

    fds[0].fd = server_socket_fd;
    fds[0].events = POLLIN;

    while (1) {
        int poll_count = poll(fds, nfds, -1);
        if (poll_count == -1) {
            perror("error poll");
            close(server_socket_fd);
            exit(EXIT_FAILURE);
        }

        if (fds[0].revents & POLLIN) {
            int client_socket = accept(server_socket_fd, (struct sockaddr*)&client_addr, &addr_len);
            if (client_socket == -1) {
                perror("error accept socket");
                continue;
            }

            int i;
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].fd == -1) {
                    clients[i].fd = client_socket;
                    clients[i].bytes_read = 0;
                    clients[i].bytes_written = 0;
                    fds[i + 1].fd = client_socket;
                    fds[i + 1].events = POLLIN | POLLOUT;
                    printf("new connection from: %s:%d (fd=%d)\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), client_socket);
                    break;
                }
            }
            if (i == MAX_CLIENTS) {
                printf("too many clients\n");
                close(client_socket);
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (fds[i + 1].fd == -1) continue;

            Client *client = &clients[i];

            if (fds[i + 1].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                printf("error on client (fd=%d): ", client->fd);
                if (fds[i + 1].revents & POLLERR) {
                    printf("POLLERR ");
                }
                if (fds[i + 1].revents & POLLHUP) { 
                    printf("POLLHUP ");
                }
                if (fds[i + 1].revents & POLLNVAL) {
                    printf("POLLNVAL ");
                }
                printf("\n");
                close(client->fd);
                client->fd = -1;
                fds[i + 1].fd = -1;
                continue;
            }

            if (fds[i + 1].revents & POLLIN) {
                ssize_t bytes_read = read(client->fd, client->buffer, BUFFER_SIZE);
                if (bytes_read <= 0) {
                    if (bytes_read == 0) {
                        printf("client disconnected (fd=%d)\n", client->fd);
                    } else {
                        perror("read error");
                    }
                    close(client->fd);
                    client->fd = -1;
                    fds[i + 1].fd = -1;
                    continue;
                }
                client->bytes_read = bytes_read;
                client->bytes_written = 0;
            }

            if (fds[i + 1].revents & POLLOUT) {
                if (client->bytes_read == 0) {
                    continue;
                }
                ssize_t bytes_to_write = client->bytes_read - client->bytes_written;
                if (bytes_to_write > 0) {
                    ssize_t bytes_written = write(client->fd, client->buffer + client->bytes_written, bytes_to_write);
                    if (bytes_written <= 0) {
                        perror("write error");
                        close(client->fd);
                        client->fd = -1;
                        fds[i + 1].fd = -1;
                        continue;
                    }
                    client->bytes_written += bytes_written;
                }
            }
        }
    }

    close(server_socket_fd);
    return EXIT_SUCCESS;
}