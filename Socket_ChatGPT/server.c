#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 8888
#define MAX_CONN 10
#define BUFFER_SIZE 128

void error(char *message) {
    perror(message);
    exit(1);
}

struct client {
    char name[BUFFER_SIZE];
    int fd;
};

struct client clients[MAX_CONN];
int num_clients = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void send_to_all(char *message, int sender_fd) {
    pthread_mutex_lock(&mutex);

    for (int i = 0; i < num_clients; i++) {
        if (clients[i].fd != sender_fd) {
            char buffer[BUFFER_SIZE];
            sprintf(buffer, "%s: %s", clients[sender_fd].name, message);
            if (write(clients[i].fd, buffer, strlen(buffer)) < 0) {
                error("Error sending message to client");
            }
        }
    }

    pthread_mutex_unlock(&mutex);
}

void *handle_connection(void *arg) {
    int client_fd = *(int*)arg;
    char buffer[BUFFER_SIZE];

    if (read(client_fd, clients[client_fd].name, BUFFER_SIZE) < 0) {
        error("Error reading client name");
    }

    printf("New client connected: %s\n", clients[client_fd].name);

    pthread_mutex_lock(&mutex);
    clients[num_clients].fd = client_fd;
    num_clients++;
    pthread_mutex_unlock(&mutex);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = read(client_fd, buffer, BUFFER_SIZE);
        if (bytes_read <= 0) {
            printf("Client disconnected: %s\n", clients[client_fd].name);
            pthread_mutex_lock(&mutex);
            for (int i = 0; i < num_clients; i++) {
                if (clients[i].fd == client_fd) {
                    for (int j = i; j < num_clients - 1; j++) {
                        clients[j] = clients[j+1];
                    }
                    num_clients--;
                    break;
                }
            }
            pthread_mutex_unlock(&mutex);
            close(client_fd);
            return NULL;
        }
        send_to_all(buffer, client_fd);
    }
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        error("Error creating server socket");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        error("Error binding server socket");
    }

    if (listen(server_fd, MAX_CONN) < 0) {
        error("Error listening on server socket");
    }

    printf("Server listening on %s:%d\n", SERVER_ADDRESS, SERVER_PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            error("Error accepting client connection");
        }
    }
}