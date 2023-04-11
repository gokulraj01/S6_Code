#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 8888
#define BUFFER_SIZE 128

void error(char *message) {
    perror(message);
    exit(1);
}

int main() {
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        error("Error creating client socket");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server_addr.sin_port = htons(SERVER_PORT);

    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        error("Error connecting to server");
    }

    char name[BUFFER_SIZE];
    printf("Enter your name: ");
    fgets(name, BUFFER_SIZE, stdin);
    name[strcspn(name, "\n")] = 0;

    if (write(client_fd, name, strlen(name)) < 0) {
        error("Error sending name to server");
    }

    while (1) {
        char message[BUFFER_SIZE];
        printf("Enter your message: ");
        fgets(message, BUFFER_SIZE, stdin);
        message[strcspn(message, "\n")] = 0;

        if (write(client_fd, message, strlen(message)) < 0) {
            error("Error sending message to server");
        }
    }

    close(client_fd);
    return 0;
}