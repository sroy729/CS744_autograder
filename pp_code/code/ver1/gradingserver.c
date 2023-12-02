#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MAX_BUFFER_SIZE 1024
#define MAX_FILE_SIZE 4

// Function to compile and run the code
int compileAndRun(const char* sourceFile) {
    int fd, fd1, fd2;
    int n1, n2, ret;

    char c1[10];
    char c2[10];

    int compileResult = system("gcc program.c -o executable 2> compileError.txt");
    printf("compileResult:%d\n", compileResult);

    if (compileResult != 0) {
        // fprintf(stderr, "COMPILER ERROR\n");
        // printf("compile return\n");
        return 0;
    }

    int executionResult = system("./executable > runtimeError.txt");

    if (executionResult != 0) 
    {
        // fprintf(stderr, "RUNTIME ERROR\n");
        system("g++ wrapper.cpp -o wrapper");
        system("./wrapper ./executable");
        return 1;
    }
    else if(executionResult == 0)
    {
        fd1 = open("runtimeError.txt", O_RDWR, 0644);
        fd = open("output.txt", O_CREAT | O_RDWR, 0644);
        fd2 = open("out.txt", O_RDWR, 0644);
	    if (fd1 < 0 || fd < 0 || fd2 < 0) 
        {
	        exit(1);
	    }
        int count = 0;
        while(1) 
        {
            n1 = read(fd1, c1, 1);
            n2 = read(fd2, c2, 1);
            // printf("\n%d %d %s %s\n", n1, n2, c1, c2);

            if (n1 < 0 || n2 < 0) {
                perror("Error reading files");
                close(fd1);
                close(fd2);
                exit(EXIT_FAILURE);
            }

            // Compare the read data
            if (n1 != n2 || memcmp(c1, c2, n1) != 0) {
                // printf("Difference found on line %d\n", count + 1);
                ret = 3;
                close(fd1);
                close(fd2);
                break;
            }
            ret = 2;
            // Check for end of file
            if (n1 == 0) {
                break;
            }
            else
            {
                write(fd, c1, 1);
            }
            count++;
        }
        close(fd);
    }
    return ret;
}

int main(int argc, char* argv[]) {

    int fd1, result;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port><numLoop>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    // int numLoop = atoi(argv[2]);

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in serverAddress, clientAddr;
    bzero((char *)&serverAddress, sizeof(serverAddress));
    socklen_t addrLen = sizeof(struct sockaddr_in);
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int iSetOption = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&iSetOption, sizeof(iSetOption));

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Binding failed");
        close(serverSocket);
        return -1;
    }

    if(listen(serverSocket, 50) != 0)
    {
        perror("Listen failed");
        close(serverSocket);
        return -1;
    }

    printf("Server listening on port %d\n", port);

    while (1) 
    {
        // Accept a client connection
        int clientSocket = accept(serverSocket,  (struct sockaddr *)&clientAddr, &addrLen);
        if (clientSocket == -1) {
            perror("Accept failed");
            continue; // Continue to the next connection
        }

        printf("Accepted connection from %d\n", ntohs(clientAddr.sin_port)); //from %s:, inet_ntoa(clientAddr.sin_addr)
        // Receive and echo messages
        while(1) 
        {
            char buffer[MAX_BUFFER_SIZE];
            char runtimebuff[MAX_BUFFER_SIZE];
            char compilebuff[MAX_BUFFER_SIZE];
            FILE* sourceFile = fopen("program.c", "wb");
            if(!sourceFile)
            {
                perror("Error opening file\n");
                return -1;
            }

            char file_size[MAX_FILE_SIZE];
            int fs = recv(clientSocket, file_size, sizeof(file_size), 0);
            if (fs <= 0) {
                close(clientSocket);
                break;
            }
            int fileSize;
            memcpy(&fileSize, file_size, sizeof(file_size));
            printf("fileSize: %d\n", fileSize);
            size_t bytesRead = 0;
            while(1)
            {
                size_t bytes_recvd = recv(clientSocket, buffer, MAX_BUFFER_SIZE, 0);
                bytesRead = bytesRead + bytes_recvd;
                printf("bytesRead: %ld bytes_recvd: %ld\n", bytesRead, bytes_recvd);
                if(bytes_recvd < 0)
                {
                    perror("Error receiving file data\n");
                    fclose(sourceFile);
                    close(clientSocket);
                    return -1;
                }
                fwrite(buffer, 1, bytes_recvd, sourceFile);
                bzero(buffer, MAX_BUFFER_SIZE);
                if(bytesRead >= fileSize)
                {
                	break;
                }
            }
            fclose(sourceFile);
            printf("before calling compile function\n");
            result = compileAndRun("program.c");
            printf("result: %d\n", result);

            if (result == 0) {
                char msg[MAX_BUFFER_SIZE] = "COMPILE ERROR";
                fd1 = open("compileError.txt", O_RDONLY, 0644);
                read(fd1, compilebuff, sizeof(compilebuff));
                strcat(msg, compilebuff);
                printf("compile time error value: %s\n", msg);
                send(clientSocket, msg, sizeof(msg), 0); 
                bzero(msg, MAX_BUFFER_SIZE);
                bzero(compilebuff, MAX_BUFFER_SIZE);
            }
            else if (result == 1) {
                char msg[MAX_BUFFER_SIZE] = "RUNTIME ERROR";
                fd1 = open("runError.txt", O_RDONLY, 0644);
                read(fd1, runtimebuff, sizeof(runtimebuff));
                strcat(msg, runtimebuff);
                printf("runtime error value: %s\n", msg);
                send(clientSocket, msg, sizeof(msg), 0);
                bzero(msg, MAX_BUFFER_SIZE);
                bzero(runtimebuff, MAX_BUFFER_SIZE);
            }
            else if (result == 2) 
            {
                char msg[MAX_BUFFER_SIZE] = "PASS";
                fd1 = open("output.txt", O_RDONLY, 0644);
                read(fd1, buffer, sizeof(buffer));
                strcat(msg, buffer);
                printf("pass output value: %s\n", msg);
                send(clientSocket, msg, sizeof(msg), 0);
                printf("\nPASS\n");
                bzero(msg, MAX_BUFFER_SIZE);
                bzero(buffer, MAX_BUFFER_SIZE);
            }
            else if (result == 3) 
            {
                char msg[MAX_BUFFER_SIZE] = "OUTPUT ERROR";
                fd1 = open("runtimeError.txt", O_RDONLY, 0644);
                read(fd1, runtimebuff, sizeof(runtimebuff));
                strcat(msg, runtimebuff);
                printf("Fail output value: %s", msg);
                send(clientSocket, msg, sizeof(msg), 0);
                printf("\nOUTPUT ERROR\n");
                bzero(msg, MAX_BUFFER_SIZE);
                bzero(runtimebuff, MAX_BUFFER_SIZE);
            }
            printf("server loop completed\n");
        }
        close(clientSocket);
        printf("Client completed\n");
    }
    close(serverSocket);
    return 0;
}
