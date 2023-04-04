// Author: Gokul Raj, 235, R6A, CSE
// UDP Time Server: Server Handler

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

#define IP_ADDR "127.0.0.1"
#define IP_PORT 8080

void error(char *msg){
    printf("[ERROR] %s\nExiting...\n", msg);
    exit(EXIT_FAILURE);
}

void main(){
    // Configure the socket parameters
    struct sockaddr_in saddr, caddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(IP_PORT);
    inet_pton(AF_INET, IP_ADDR, &saddr.sin_addr);
    
    // Create a socket and obtain fd
    int sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sfd == -1)
        error("Failed to create socket!!");
    
    // Bind to Network
    if(bind(sfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in)) == -1)
        error("Cannot bind to host!!");
    else
        printf("Timeserver started!!\nWaiting for request...\n");
    
    // Wait for request, then respond
    int r;
    time_t currTime;
    socklen_t caddrlen = (socklen_t) sizeof(caddr);
    while(1){
        r = 0;
        recvfrom(sfd, &r, sizeof(int), 0, (struct sockaddr*)&caddr, &caddrlen);
        if(r == 1){
            currTime = time(NULL);
            if(sendto(sfd, &currTime, sizeof(currTime), 0, (struct sockaddr *)&caddr, sizeof(saddr)) == -1){
                perror("Err");
                exit(EXIT_FAILURE);
            }
            else
                printf("Time sent!!\n");
        }
    }
}
