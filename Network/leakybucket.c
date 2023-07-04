#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define QUEUE_SIZE 25
#define MAX_DATA 5

int req_n, req_rate, pro_rate, temp;
double req_delay, pro_delay;

// Queue to store requests
int *data, front, back;
int push(int q){
    if((back+1)%QUEUE_SIZE != front){
        data[back] = q;
        back = (back+1)%QUEUE_SIZE;
        return 1;
    }
    else return 0;
}
int pop(){
    if(front == 0 && back == 0) return -1;
    temp = data[front];
    front = (front+1)%QUEUE_SIZE;
    if(front == back)
        front = back = 0;
    return temp;
}

void *addRequest(){
    int curr = 0, data;
    while(curr < req_n){
        // Get a random delay between 0.00 and 1.99 sec
        data = rand()%MAX_DATA;
        clock_t start = clock();
        for(int i=0; i<data; i++){
            if(push(data))
                printf("[REQ:%d] %d bytes added to queue\n", curr, data);
            else
                printf("[REQ:%d] QUEUE IS FULL. Request dropped!!\n", curr);
        }
        while((clock()-start)/CLOCKS_PER_SEC <= req_delay);
        curr++;
    }
}

void *processRequest(){
    int curr = 0, data = -1;
    while(curr < req_n){
        clock_t start = clock();
        data = pop();
        if(data > -1)
            printf("[SEND:%d] Sending %d bytes\n", curr, data);
        // Apply delay before processing next packet
        while((clock()-start)/CLOCKS_PER_SEC <= pro_delay);
        curr++;
    }
}

void main(){
    // Seed the random generator with time
    srand(time(NULL));
    // Input parameters for leaky bucket
    printf("No: of reuqests: ");
    scanf("%d", &req_n);
    printf("Request Rate [reqs/sec]: ");
    scanf("%d", &req_rate);
    printf("Process Rate [reqs/sec]: ");
    scanf("%d", &pro_rate);
    req_delay = (double) 1/req_rate;
    pro_delay = (double) 1/pro_rate;

    data = malloc(sizeof(int)*QUEUE_SIZE);
    pthread_t add, process;
    pthread_create(&add, NULL, addRequest, NULL);
    pthread_create(&process, NULL, processRequest, NULL);

    // Wait for threads to terminate
    pthread_join(add, NULL);
    pthread_join(process, NULL);
    free(data);
}
