#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define IP_ADDR "127.0.0.1"
#define IP_PORT 8080
#define TIMEOUT 2
#define DROP_RATE 9

struct sockaddr_in sock;

struct Packet{
    int i;
    char ack;
};

char dropCheck(){
    if(random()%100 >= DROP_RATE)
        return 0;
    else
        return 1;
}

void main(){
    srandom(time(NULL));
    // Initialize Socket Connection
    sock.sin_family = AF_INET;
    sock.sin_port = htons(IP_PORT);
    inet_pton(AF_INET, IP_ADDR, &sock.sin_addr);
    int sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sfd == -1) perror("Socket Err");
    int ack = 1;

    // Send Hello
    if(sendto(sfd, &ack, sizeof(int), 0, (struct sockaddr*) &sock, sizeof(sock)) > 0)
        printf("Client Started!!\n");

    // Start recieveing
    socklen_t addrlen = (socklen_t) sizeof(sock);
    struct Packet buff;
    if(recvfrom(sfd, &buff, sizeof(buff), 0, (struct sockaddr*) &sock, &addrlen) > 0){
        if(!dropCheck()){
            if(sendto(sfd, &buff.i, sizeof(buff.i), 0, (struct sockaddr*) &sock, (socklen_t) sizeof(sock)) <= 0){
                perror("Ack Err");
                exit(EXIT_FAILURE);
            }
            else printf("[ACK] Packet %d\n", buff.i);
        }
        else printf("[DROP] Packet %d\n", buff.i);
    }
}
