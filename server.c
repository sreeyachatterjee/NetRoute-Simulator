//server.c
// server.c
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 8080
#define MAX_NODES 10
#define INF 99999

int graph[MAX_NODES][MAX_NODES] = {
    {0, 4, INF, INF, INF, INF, INF, 8, INF},
    {4, 0, 8, INF, INF, INF, INF, 11, INF},
    {INF, 8, 0, 7, INF, 4, INF, INF, 2},
    {INF, INF, 7, 0, 9, 14, INF, INF, INF},
    {INF, INF, INF, 9, 0, 10, INF, INF, INF},
    {INF, INF, 4, 14, 10, 0, 2, INF, INF},
    {INF, INF, INF, INF, INF, 2, 0, 1, 6},
    {8, 11, INF, INF, INF, INF, 1, 0, 7},
    {INF, INF, 2, INF, INF, INF, 6, 7, 0}
};

void dijkstra(int src, int dest, char* result) {
    int dist[MAX_NODES], visited[MAX_NODES];
    int parent[MAX_NODES];
    for (int i = 0; i < MAX_NODES; i++) {
        dist[i] = INF;
        visited[i] = 0;
        parent[i] = -1;
    }

    dist[src] = 0;
    for (int i = 0; i < MAX_NODES - 1; i++) {
        int min = INF, u;
        for (int j = 0; j < MAX_NODES; j++) {
            if (!visited[j] && dist[j] <= min) {
                min = dist[j];
                u = j;
            }
        }

        visited[u] = 1;

        for (int v = 0; v < MAX_NODES; v++) {
            if (!visited[v] && graph[u][v] && dist[u] != INF &&
                dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }

    int path[MAX_NODES], count = 0;
    for (int v = dest; v != -1; v = parent[v])
        path[count++] = v;

    sprintf(result, "Distance: %d, Path: ", dist[dest]);
    for (int i = count - 1; i >= 0; i--) {
        char node[5];
        sprintf(node, "%d", path[i]);
        strcat(result, node);
        if (i > 0) strcat(result, " -> ");
    }
}

void handle_client(int sock) {
    char buffer[1024];
    int src, dest;

    read(sock, buffer, sizeof(buffer));
    src = atoi(buffer);

    read(sock, buffer, sizeof(buffer));
    dest = atoi(buffer);

    char result[1024] = {0};
    dijkstra(src, dest, result);

    write(sock, result, sizeof(result));
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        perror("Socket bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, 5) != 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    addr_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
    handle_client(client_sock);

    close(client_sock);
    close(server_sock);
    return 0;
}
