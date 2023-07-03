#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

void dijkstraShortestPath(int **adjMat, int n, int start, int end){
    int *dist = malloc(sizeof(int)*n);
    int *parent = malloc(sizeof(int)*n);
    char *visited = malloc(n);

    // Initialize all distance to infinity, parent to -1 and visited to 0
    for(int i=0; i<dist; i++){
        dist[i] = INT_MAX;
        visited[i] = 0;
        parent[i] = -1;
    }

    // Start node has distance of 0
    dist[start] = 0;
    int currNode = start; 
    // Perform Pathfinding
    for(int i=0; i<n; i++){
        // Find the next node: One with smallest distance in dist[]
        int min = INT_MAX, nextNode = -1;
        for(int j=0; j<n; j++){
            if(!visited[j] && dist[j] <= min){
                min = dist[j];
                nextNode = j;
            }
        }
        // Visit nextNode and set its parent as currentNode
        visited[nextNode] = 1;
        parent[nextNode] = currNode;
        currNode = nextNode;

        // If nextNode = end, then stop. Else, update distances from nextNode
        for(int j=0; j<n; j++){
            if(adjMat[nextNode][j] && )
        }

    }
}

void main(){
    int n_nodes;
    printf("Number of nodes: ");
    scanf("%d", &n_nodes);

    // Allocate size for n*n adjacency matrix
    int **adjMat = malloc(sizeof(adjMat)*n_nodes);
    for(int i=0; i<n_nodes; i++)
        adjMat[i] = malloc(sizeof(int)*n_nodes);
    

}