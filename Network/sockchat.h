#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#define IP_ADDR "127.0.0.1"
#define IP_PORT 8080
#define LISTEN_QUEUE_SIZE 5
#define NAME_SIZE 32
#define MSG_SIZE 256

struct User{
    int fd;
    int id;
    pthread_t tid;
    char name[NAME_SIZE];
};

struct Msg{
    char author[NAME_SIZE];
    char msg[MSG_BATCH];
};