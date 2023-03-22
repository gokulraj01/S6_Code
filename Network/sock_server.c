#include <stdio.h>
#include <stdlib.h>
#include "sockchat.h"

struct sockaddr_in saddr;
struct User **activeConn = NULL;
int sfd, nconn, iconn;
pthread_t acc;

void cleanExit(){
    printf("\nQuitting...\n");
    if(activeConn != NULL){
        pthread_cancel(acc);
        for(int i=0; i<LISTEN_QUEUE_SIZE; i++){
            if(activeConn[i] != NULL){
                pthread_cancel(activeConn[i]->tid);
                close(activeConn[i]->fd);
            }
        }
        close(sfd);
    }
    exit(EXIT_SUCCESS);
}

void error(char *msg){
    printf("[ERROR] %s\n", msg);
    cleanExit();
}

void broadcastMsg(struct Msg *msg, int id){
    for(int i=0; i<LISTEN_QUEUE_SIZE; i++){
        if(i != id && activeConn[i] != NULL)
            write(activeConn[i]->fd, msg, sizeof(struct Msg));
    }
}

void *asyncRead(void *vpos){
    int *pos = vpos;
    int i = 0;
    struct Msg *buf = malloc(sizeof(struct Msg));
    printf("\nThread started for index %d [User: %d, FD: %d]\n", *pos, activeConn[*pos]->id, activeConn[*pos]->fd);
    fflush(stdout);
    while(1){
        int status = read(activeConn[*pos]->fd, buf, sizeof(struct Msg));
        if(status > 0){
            if(status == sizeof(struct Msg)){
                printf("%s: %s\n", buf->author, buf->msg);
                broadcastMsg(buf, *pos);
                printf("User %d gave %d bytes\n", activeConn[*pos]->id, status);
            }
        }
        else{
            // Closed gracefully
            if(status == 0)
                printf("User %d left!!\n", activeConn[*pos]->id);
            // Closed with error
            else
                printf("{Status %d} User %d read error!!\n", status, activeConn[*pos]->id);
            close(activeConn[*pos]->fd);
            free(activeConn[*pos]);
            activeConn[*pos] = NULL;
            nconn--;
            pthread_exit(EXIT_SUCCESS);
        }
    }
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
                        if(read(t, u->name, sizeof(u->name)) > 0){
                            activeConn[i] = u;
                            printf("New connection: User %d [fd:%d]\n", iconn, t);
                            // Spawn thread to listen to this connection
                            int *thread_ind = malloc(sizeof(int));
                            *thread_ind = i;
                            pthread_create(&u->tid, NULL, asyncRead, thread_ind);
                        }
                        else{
                            free(u);
                            printf("[WARN] Username not recieved!! Declining..\n");
                        }
                        break;
                    }
                }
            }
        }
    }
}

void main(){
    signal(SIGINT, cleanExit);
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
    pthread_create(&acc, NULL, asyncAccept, NULL);

    printf("Server Initalized!!\nPress 'Ctrl+C' to EXIT...\n");

    // Wait for signal, then close socket and exit
    while(1) sleep(1);
}
