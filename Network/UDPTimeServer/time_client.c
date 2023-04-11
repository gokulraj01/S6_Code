// Author: Gokul Raj, 235, R6A, CSE
// UDP Time Server: Client

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
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(IP_PORT);
    inet_pton(AF_INET, IP_ADDR, &saddr.sin_addr);
    
    // Create a socket and obtain fd
    int sfd = socket(AF_INET, SOCK_DGRAM, 0);
    printf("Socket created!!\n");
    if(sfd == -1)
        error("Failed to create socket!!");
    
    // Get the current time
    time_t currTime;
    socklen_t len = sizeof(saddr);
    
    printf("Requesting time.. ");
    int r = 1;
    if(sendto(sfd, &r, sizeof(int), 0, (struct sockaddr *)&saddr, sizeof(saddr)) <= 0){
        perror("Err");
        exit(EXIT_FAILURE);
    }
    else
        printf("Request Successful\n");
    if(recvfrom(sfd, &currTime, sizeof(currTime), MSG_WAITALL, (struct sockaddr *)&saddr, &len) > 0);
        printf("Got time as %s\n", ctime(&currTime));
}
