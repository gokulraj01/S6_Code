// Author: Gokul Raj, 235, R6A, CSE
// FTP: Client
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
        printf("[ERR] No output file specified!!\nUse as:\n\t%s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Initialize socket address
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(IP_PORT);
    inet_pton(AF_INET, IP_ADDR, &addr.sin_addr);

    // Create a socket connection
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(connect(sfd, (struct sockaddr*) &addr, sizeof(addr)) != 0) err("ConnectErr");
    else printf("Connected!!\n");

    // Start recieveing chunks from server, store locally
    FILE *outfile = fopen(argv[1], "wb");
    size_t len = 0, ind = 0;
    while(recv(sfd, &dataPacket, sizeof(dataPacket), 0) > 0){
        len += dataPacket.len;
        if(fwrite(dataPacket.data, 1, dataPacket.len, outfile) > 0)
            printf("Got chunk %ld - %ld bytes\n", ind++, dataPacket.len);
    }
    close(sfd);
    printf("Receieved Total: %ldbytes\n", len);
}