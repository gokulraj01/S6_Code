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
#define TIMEOUT 1

struct sockaddr_in sock, caddr;
socklen_t addrlen;
int sfd, left, right, packet_n, window_n, ind=0;

struct Packet{
    int i;
    char ack;
};

struct Packet **data;

void err(char* msg){
    perror(msg);
    close(sfd);
    exit(EXIT_FAILURE);
}

void *recieveAck(){
    int ackno;
    while(right < packet_n){
        if(recvfrom(sfd, &ackno, sizeof(ackno), 0, (struct sockaddr*) &caddr, &addrlen) >= 0){
            data[ackno]->ack = 1;
            // Slide window to meet next unacknowledged packet
            while(data[left]->ack){ left++; right++; }
        }
    }
}

void transmit(int *curr){
    if(sendto(sfd, data[*curr], sizeof(struct Packet), 0, (struct sockaddr*) &caddr, (socklen_t) sizeof(caddr)) <= 0)
        err("Send Err");
    
    printf("Progress: %d%% [SENT:%d] Packet %d\n", (*curr)*100/packet_n, ind++, *curr);
    (*curr)++;
}

void *sendPacket(){
    int curr = left;
    // Wait for hello..
    int hello;
    time_t wait_start;
    struct Packet endPacket = {-1, 0};
    printf("Waiting for client...\n");
    addrlen = (socklen_t) sizeof(caddr);
    if(recvfrom(sfd, &hello, sizeof(hello), 0, (struct sockaddr*) &caddr, &addrlen) < sizeof(int))
        err("No client");
    printf("Client Connected | %s:%d\n", inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));
    
    while(curr < packet_n){
        // Send packet if within window
        if(curr <= right) transmit(&curr);
        else{
            printf("Waiting %d sec for ACK...\n", TIMEOUT);
            wait_start = time(NULL);
            while(curr >= right){
                if(time(NULL)-wait_start >= TIMEOUT){
                    printf("ACK Timeout: Resending Packets %d:%d\n", left, right);
                    curr = left;
                    int temp_r = right;
                    while(curr <= temp_r) transmit(&curr);
                    break;
                }
            }
        }
    }

    // Send End Packet to close transfer
    if(sendto(sfd, &endPacket, sizeof(struct Packet), 0, (struct sockaddr*) &caddr, (socklen_t) sizeof(caddr)) <= 0)
        err("Transfer End Error");
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
        err("Bind Err");
    
    // Start Send and Recieve Threads
    pthread_t st, rt;
    pthread_create(&st, NULL, sendPacket, NULL);
    pthread_create(&rt, NULL, recieveAck, NULL);
    
    // Wait for completion
    pthread_join(st, NULL);
    pthread_join(rt, NULL);
    printf("Transfer complete!!\n");

    // Close all connections
    close(sfd);
}
