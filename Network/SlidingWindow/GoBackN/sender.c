#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

#define IP_ADDR "127.0.0.1"
#define IP_PORT 8080
#define TIMEOUT 2

struct sockaddr_in sock, caddr;
int sfd, left, right, packet_n, window_n, ind=0;

struct Packet{
    int i;
    char ack;
};

struct Packet **data;

void *recieveAck(){
    int ackno;
    socklen_t addrlen = (socklen_t) sizeof(caddr);
    while(1){
        if(recvfrom(sfd, &ackno, sizeof(ackno), 0, (struct sockaddr*) &caddr, &addrlen) <= 0){
            data[ackno]->ack = 1;
            // Slide window to meet next unacknowledged packet
            while(data[left]->ack){ left++; right++; }
        }
    }
}

void transmit(int *curr){
    if(sendto(sfd, data[*curr], sizeof(struct Packet), 0, (struct sockaddr*) &sock, (socklen_t) sizeof(sock)) <= 0){
        perror("Send Err");
        exit(EXIT_FAILURE);
    }
    printf("[SENT:%d] Packet %d\n", ind++, *curr);
    (*curr)++;
}

void *sendPacket(){
    int curr = left;
    while(curr < packet_n){
        // Send packet if within window
        if(curr <= right) transmit(&curr);
        else{
            printf("Waiting for ACK...\n");
            sleep(TIMEOUT);
            // Check if situation has changed?
            // Else resend all packets in current window
            if(curr > right){
                curr = left;
                while(curr <= right) transmit(&curr);
            }
        }
    }
}

void main(){
    printf("How many packets? ");
    scanf("%d", &packet_n);
    printf("Window Size: ");
    scanf("%d", &window_n);
    
    // Allocate packet buffer
    data = malloc(sizeof(data)*packet_n);
    for(int i=0; i<packet_n; i++){
        data[i] = malloc(sizeof(struct Packet));
        data[i]->i = i;
        data[i]->ack = 0;
    }
    
    // Initialize Window
    left = 0;
    right = window_n-1;
    
    // Initialize Socket Connection
    sock.sin_family = AF_INET;
    sock.sin_port = htons(IP_PORT);
    inet_pton(AF_INET, IP_ADDR, &sock.sin_addr);
    sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sfd == -1) perror("Socket Err");
    if(bind(sfd, (struct sockaddr *)&sock, sizeof(struct sockaddr_in)) == -1)
        perror("Bind Err");
    
    // Wait for hello..
    
    
    // Start Send and Recieve Threads
    pthread_t st, rt;
    pthread_create(&st, NULL, sendPacket, NULL);
    pthread_create(&rt, NULL, recieveAck, NULL);
    
    // Wait for completion
    pthread_join(st, NULL);
    pthread_join(rt, NULL);
    
    // Close all connections
    close(sfd);
}
