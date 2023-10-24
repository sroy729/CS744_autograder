#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MAX_BUFFER_SIZE 4096

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <serverIP:port> <sourceCodeFile>\n", argv[0]);
        return 1;
    }

    char* serverAddressStr = strtok(argv[1], ":");
    char* serverPortStr = strtok(NULL, ":");
    if (!serverAddressStr || !serverPortStr) {
        fprintf(stderr, "Invalid server address format.\n");
        return 1;
    }

    char* sourceCodeFile = argv[2];

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(atoi(serverPortStr));
    if (inet_pton(AF_INET, serverAddressStr, &serverAddress.sin_addr) <= 0) {
        perror("Invalid server address");
        return 1;
    }

    if (connect(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Connection failed");
        return 1;
    }

    FILE* sourceFile = fopen(sourceCodeFile, "rb");
    if (sourceFile == NULL) {
        perror("Failed to open source code file");
        close(serverSocket);
        return 1;
    }

    fseek(sourceFile, 0, SEEK_END);
    long fileSize = ftell(sourceFile);
    fseek(sourceFile, 0, SEEK_SET);

    char buffer[MAX_BUFFER_SIZE];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), sourceFile)) > 0) {
        send(serverSocket, buffer, bytesRead, 0);
    }

    fclose(sourceFile);

    char response[MAX_BUFFER_SIZE];
    int bytesReceived = recv(serverSocket, response, sizeof(response), 0);
    if (bytesReceived > 0) {
        response[bytesReceived] = '\0';
        printf("Response : %s\n", response);
    }

    char response2[MAX_BUFFER_SIZE];
    int bytesReceived2 = recv(serverSocket, response2, sizeof(response2), 0);
    if (bytesReceived2 > 0) {
        response2[bytesReceived2] = '\0';
        printf("Response2 : %s\n", response2);
    }

    close(serverSocket);
    return 0;
}
