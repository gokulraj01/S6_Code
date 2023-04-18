// Author: Gokul Raj, 235, R6A, CSE
// TCP String Reverse: Client

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define IP_ADDR "127.0.0.1"
#define IP_PORT 8080
#define RECIEVE_SIZE 128

int sfd;

void err(char *msg){
    close(sfd);
    perror(msg);
    exit(EXIT_FAILURE);
}

void cleanup(){
    close(sfd);
    printf("Exit\n");
    exit(EXIT_SUCCESS);
}

void main(){
    signal(SIGINT, cleanup);
    // Setup Socket
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    sock.sin_port = htons(IP_PORT);
    inet_pton(AF_INET, IP_ADDR, &sock.sin_addr);
    sfd = socket(AF_INET, SOCK_STREAM, 0);

    // Get a connection
    socklen_t addrlen = sizeof(sock);
    if(connect(sfd, (struct sockaddr*) &sock, addrlen) < 0) err("Connect Err");
    else printf("Connected!!\n");

    // Write to socket
    while(1){
        char* buf = malloc(RECIEVE_SIZE);
        printf("> ");
        scanf("%s", buf);
        if(write(sfd, buf, 128)){
            if(read(sfd, buf, 128))
                printf("Server: %s\n", buf);
        }
    }    
}
