#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#define IP_ADDR "127.0.0.1"
#define IP_PORT 8080
#define LISTEN_QUEUE_SIZE 5

void error(char *msg){
    printf("[ERROR] %s\nExiting...\n", msg);
    exit(EXIT_FAILURE);
}

void main(){
    // Configure the socket parameters
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(IP_PORT);
    inet_pton(AF_INET, IP_ADDR, &saddr.sin_addr);
    
    // Create a socket and obtain fd
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd == -1)
        error("Failed to create socket!!");
    
    // Connect to socket
    if(connect(sfd, (struct sockaddr*)&saddr, (socklen_t)sizeof(struct sockaddr_in)) == -1)
        error("Connection failed");
    else
        printf("Connected to %s:%d\n", IP_ADDR, IP_PORT);
    
    char* buf = malloc(128);
    strncpy(buf, "Hello Sockets!!", 128);
    int n = write(sfd, buf, 128);
    printf("Status: %d\n", n);
    // Close socket
    close(sfd);
}
