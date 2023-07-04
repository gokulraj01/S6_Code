// Author: Gokul Raj, 235, R6A, CSE
// FTP: Server
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define CHUNK_SIZE 2048
#define IP_ADDR "127.0.0.1"
#define IP_PORT 8080

struct DataUnit{
    char data[CHUNK_SIZE];
    size_t len;
} dataPacket;

void err(char *msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

void main(int argc, char **argv){
    if(argc < 2){
        printf("[ERR] No file selected to send!!\nUse as:\n\t%s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Initialize socket address
    struct sockaddr_in addr, caddr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(IP_PORT);
    inet_pton(AF_INET, IP_ADDR, &addr.sin_addr);

    // Create a socket connection
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(bind(sfd, (struct sockaddr*) &addr, sizeof(addr)) != 0) err("BindErr");
    if(listen(sfd, 1) != 0) err("ListenErr");

    // Wait for connection, then accept
    socklen_t addrlen;
    printf("Waiting for connection...\n");
    int cfd = accept(sfd, (struct sockaddr*) &caddr, &addrlen);
    
    // Open file to send, then transmit as chunks
    FILE *infile = fopen(argv[1], "rb");
    size_t len = 0, ind = 0;
    while((dataPacket.len = fread(dataPacket.data, 1, CHUNK_SIZE, infile)) > 0){
        if(send(cfd, &dataPacket, sizeof(dataPacket), 0) > -1){
            len += dataPacket.len;
            printf("Sending chunk %ld - %ld bytes\n", ind++, dataPacket.len);
        }
        else err("SendErr");
    }
    close(cfd);
    close(sfd);
    printf("Sent Total: %ldbytes\n", len);
}