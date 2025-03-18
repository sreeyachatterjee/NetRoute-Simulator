//client.c
// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX 1024

void communicate(int sock) {
    char buffer[MAX];
    printf("Enter the source node: ");
    fgets(buffer, sizeof(buffer), stdin);
    send(sock, buffer, strlen(buffer), 0);

    printf("Enter the destination node: ");
    fgets(buffer, sizeof(buffer), stdin);
    send(sock, buffer, strlen(buffer), 0);

    bzero(buffer, sizeof(buffer));
    read(sock, buffer, sizeof(buffer));
    printf("Shortest path from server: %s\n", buffer);
}

int main() {
    int sock;
    struct sockaddr_in server_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        perror("Connection to server failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server.\n");
    communicate(sock);
    close(sock);

    return 0;
}
