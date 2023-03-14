#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#define IP_ADDR "127.0.0.1"
#define IP_PORT 8080
#define LISTEN_QUEUE_SIZE 5
#define MSG_SIZE 128

struct sockaddr_in saddr;
struct User **activeConn = NULL;
int sfd, nconn, iconn;

struct User{
    int fd;
    int id;
};

void error(char *msg){
    printf("[ERROR] %s\nExiting...\n", msg);
    exit(EXIT_FAILURE);
}

void *asyncAccept(){
    // Initialize connection list
    activeConn = malloc(sizeof(activeConn)*LISTEN_QUEUE_SIZE);
    for(int i=0; i<LISTEN_QUEUE_SIZE; i++)
        activeConn[i] = NULL;

    nconn = 0;
    iconn = 0;
    while(1){
        if(nconn < LISTEN_QUEUE_SIZE){
            socklen_t addrlen = sizeof(saddr);
            int t = accept(sfd, (struct sockaddr *)&saddr, &addrlen);
            if(t == -1)
                error("Could not accept connection!!");
            else{
                nconn++; iconn++;
                for(int i=0; i<LISTEN_QUEUE_SIZE; i++){
                    if(activeConn[i] == NULL){
                        struct User *u = malloc(sizeof(struct User));
                        u->id = iconn;
                        u->fd = t;
                        activeConn[i] = u;
                        printf("New connection: User %d [fd:%d]\n", iconn, t);
                        break;
                    }
                }
            }
        }
    }
}

void *asyncRead(){
    int i = 0;
    char* buf = malloc(MSG_SIZE);
    while(1){
        if(activeConn[i] != NULL){
            int status = read(activeConn[i]->fd, buf, MSG_SIZE);
            if(status > 0)
                printf("User %d: %s\n", activeConn[i]->id, buf);
            else{
                // Closed gracefully
                if(status == 0)
                    printf("User %d left!!\n", activeConn[i]->id);
                // Closed with error
                else
                    printf("{Status %d} User %d read error!!\n", status, activeConn[i]->id);
                close(activeConn[i]->fd);
                free(activeConn[i]);
                activeConn[i] = NULL;
                nconn--;
            }
        }
    }
}

void main(){
    pthread_t rd, acc;
    // Configure the socket parameters
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(IP_PORT);
    inet_pton(AF_INET, IP_ADDR, &saddr.sin_addr);
    
    // Create a socket and obtain fd
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd == -1)
        error("Failed to create socket!!");
    
    // Bind to Network
    if(bind(sfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in))== -1)
        error("Cannot bind to host!!");
    
    // Start Listening
    if(listen(sfd, LISTEN_QUEUE_SIZE) == -1)
        error("Cannot start listening!!");
    else
        printf("Waiting for connection on %s:%d...\n", IP_ADDR, IP_PORT);
    
    // Accept connections and Read Data
    pthread_create(&rd, NULL, asyncAccept, NULL);
    pthread_create(&acc, NULL, asyncRead, NULL);

    printf("Server Initalized!!\nPress 'q' to EXIT...");
    
    // Wait for signal, then close socket and exit
    while(getchar() != 'q');
    pthread_cancel(rd);
    pthread_cancel(acc);
    close(sfd);
}
