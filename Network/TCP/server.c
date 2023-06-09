// Author: Gokul Raj, 235, R6A, CSE
// TCP String Reverse: Server

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#define IP_ADDR "127.0.0.1"
#define IP_PORT 8080
#define RECIEVE_SIZE 128

int cfd, sfd;

void cleanup(){
    printf("Exit\n");
    close(cfd);
    close(sfd);
    exit(EXIT_SUCCESS);
}

void main(){
    signal(SIGINT, cleanup);
    int l; char t;
    // Setup Socket
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    sock.sin_port = htons(IP_PORT);
    inet_pton(AF_INET, IP_ADDR, &sock.sin_addr);
    sfd = socket(AF_INET, SOCK_STREAM, 0);

    // Bind to network
    if(bind(sfd, (struct sockaddr*) &sock, (socklen_t)sizeof(sock))!=0){
        perror("Bind Err");
        exit(EXIT_FAILURE);
    }

    // Get a connection
    listen(sfd, 1);
    socklen_t addrlen = sizeof(sock);
    printf("Server Started!!\nWaiting for connection...\n");
    cfd = accept(sfd, (struct sockaddr*) &sock, &addrlen);
    printf("Connected!!\n");

    // Read from socket
    char* buf = malloc(RECIEVE_SIZE);
    while(read(cfd, buf, RECIEVE_SIZE) > 0){
        l = 0;
        printf("Got: %s\n", buf);
        while(buf[l] != 0) l++;
        for(int i=0; i<(l/2); i++){
            t = buf[i];
            buf[i] = buf[l-i-1];
            buf[l-i-1] = t;
        }
        write(cfd, buf, 128);
    }
}
