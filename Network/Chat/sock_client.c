#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sockchat.h"

int sfd;

void cleanExit(){
    printf("Quitting...");
    close(sfd);
    exit(EXIT_SUCCESS);
}

void error(char *msg){
    printf("[ERROR] %s\n", msg);
    cleanExit();
}

void *displayChat(){
    struct Msg *inbuf = malloc(sizeof(struct Msg));
    while(1){
        if(read(sfd, inbuf, sizeof(struct Msg)) == sizeof(struct Msg)){
            printf("\r[%s] %s\n> ", inbuf->author, inbuf->msg);
            fflush(stdout);
        }
    }
}

void main(){
    signal(SIGINT, cleanExit);
    // Configure the socket parameters
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(IP_PORT);
    inet_pton(AF_INET, IP_ADDR, &saddr.sin_addr);
    
    // Create a socket and obtain fd
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd == -1)
        error("Failed to create socket!!");
    
    // Connect to socket
    if(connect(sfd, (struct sockaddr*)&saddr, (socklen_t)sizeof(struct sockaddr_in)) == -1)
        error("Connection failed");
    else
        printf("Connected to %s:%d\n" , IP_ADDR, IP_PORT);

    // Initial Handshake
    char disp_name[NAME_SIZE];
    printf("DisplayName: ");
    scanf("%s", disp_name);
    if(write(sfd, disp_name, NAME_SIZE) < 0)
        error("Handshake failed");
    else
        printf("Connected to chatserver as \'%s\'\n", disp_name);

    struct Msg *buf = malloc(sizeof(struct Msg));
    strncpy(buf->author, disp_name, NAME_SIZE);

    pthread_t disp;
    pthread_create(&disp, NULL, displayChat, NULL);
    while(1){
        printf("> ");
        fgets(buf->msg, MSG_SIZE, stdin);
        if(write(sfd, buf, sizeof(struct Msg)) <= 0)
            error("Unable to write to server");
        // \033[F -> Move to start of prev line
        else
            printf("\033[FMe: %s\n", buf->msg);
    }
    // Close socket
    close(sfd);
}
