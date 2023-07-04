// Author: Gokul Raj, 235, R6A, CSE
// Link State Routing: Routing Demo
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

void printPaths(int *parent, int *dist, int start, int n){
    int *buff = malloc(sizeof(int)*n);
    printf("Dest\tWeight\tPath\n");
    for(int i=0; i<n; i++){
        int end = i;
        int top = -1;
        printf("Node%d\t%d\t", end, dist[end]);
        while(end != start && end != -1){
            buff[++top] = end;
            end = parent[end];
        }
        while(top > -1) printf("{%d} --> ", buff[top--]);
        printf("{%d}\n", end);
    }
    free(buff);
}

void dijkstraShortestPath(int **adjMat, int n, int start){
    int *dist = malloc(sizeof(int)*n);
    int *parent = malloc(sizeof(int)*n);
    char *visited = malloc(n);

    // Initialize all vectors with defaults
    for(int i=0; i<n; i++){
        dist[i] = INT_MAX;
        visited[i] = 0;
        parent[i] = -1;
    }

    // Start node has distance of 0
    dist[start] = 0;
    int prevNode = start;
    int min, currNode;

    // Perform the Pathfinding
    for(int i=0; i<n; i++){
        // Find the next node: One with smallest distance in dist[]
        min = INT_MAX, currNode = -1;
        for(int j=0; j<n; j++){
            if(!visited[j] && dist[j] <= min){
                min = dist[j];
                currNode = j;
            }
        }
        
        if(currNode == -1) break;   // Return if all feasilble paths explored
        visited[currNode] = 1;      // Visit currNode
        
        // if(currNode == end) break;  // Return if reached the end

        // Update distances from currNode
        int newDist;
        for(int j=0; j<n; j++){
            newDist = dist[currNode] + adjMat[currNode][j];
            // Update if path feasible (weight != 0) & newDist < oldDist
            if(adjMat[currNode][j] && newDist < dist[j]){
                dist[j] = newDist;
                parent[j] = currNode;
            }
        }
        // Update previous node info for parent chaining
        prevNode = currNode;
    }
    printPaths(parent, dist, start, n);
    free(dist); free(parent); free(visited);
}

void main(int argc, char** argv){
    FILE *stream = stdin;
    char fromFile = 0;
    // If file given as arg, then use it to read adjMat
    if(argc > 1){
        stream = fopen(argv[1], "r");
        fromFile = 1;
    }
    int n_nodes;
    if(!fromFile) printf("Number of nodes: ");
    fscanf(stream, "%d", &n_nodes);

    // Allocate size for n*n adjacency matrix and input elements
    int **adjMat = malloc(sizeof(adjMat)*n_nodes);
    if(!fromFile) printf("Enter Adjacency Martix\n");
    for(int i=0; i<n_nodes; i++){
        adjMat[i] = malloc(sizeof(int)*n_nodes);
        if(!fromFile) printf("> ");
        for(int j=0; j<n_nodes; j++) fscanf(stream, "%d", &adjMat[i][j]);
        if(!fromFile) putchar('\n');
    }
    if(fromFile){
        printf("[SUCCESS] Got Adjacency Matrix from file - %s\n", argv[1]);
        fclose(stream);
    }

    int s;
    printf("Who is this node? ");
    scanf("%d", &s);
    printf("\nRouting Table for Node %d\n", s);
    dijkstraShortestPath(adjMat, n_nodes, s);
}