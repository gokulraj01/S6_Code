#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define IP_ADDR "127.0.0.1"
#define IP_PORT 8080
#define RECIEVE_SIZE 128

void main(){
    // Setup Socket
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    sock.sin_port = htons(IP_PORT);
    inet_pton(AF_INET, IP_ADDR, &sock.sin_addr);
    int sfd = socket(AF_INET, SOCK_STREAM, 0);

    // Get a connection
    listen(sfd, 1);
    socklen_t addrlen = sizeof(sock);
    int cfd = connect(sfd, (struct sockaddr*) &sock, addrlen);
    printf("Connected!!\n");

    // Write to socket
    while(1){
        char* buf = malloc(RECIEVE_SIZE);
        printf("> ");
        scanf("%s", buf);
        if(write(cfd, buf, 128)){
            if(read(cfd, buf, 128))
                printf("Server: %s\n", buf);
        }
    }    
    printf("Exit\n");
    close(cfd);
    close(sfd);
}