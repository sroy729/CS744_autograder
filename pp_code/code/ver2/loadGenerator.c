#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <pthread.h>

#define MAX_BUFFER_SIZE 1024
#define MAX_FILE_SIZE 4

struct sockaddr_in serverAddress;
struct timeval timeout;
char* sourceCodeFilePath;
long double avgResponseTime = 0.0;
long double responseTime = 0.0;
int numSuccessfulResp = 0;
int numTimeout = 0;

void* createClient(void* arg)
{
    // struct timeval Tsend, Trecv;
    // long double avgResponseTime = 0.0;
    // int numSuccessfulResp = 0;
    // int numTimeout = 0;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) 
    {
        perror("Socket creation failed");
        return (void*)1;
    }

    int tries = 0;
        while(1)
        {
            if(connect(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == 0) 
                break;
            sleep(1);
            tries += 1;
            if(tries == 3)
            {
                perror("Connection failed");
                return (void*)1;
            }
        }
        
        char buffer[MAX_BUFFER_SIZE];
        size_t bytesRead;
        FILE* sourceFile = fopen(sourceCodeFilePath, "rb");
        if (sourceFile == NULL) 
        {
            printf("heluulo\n");
            perror("Failed to open source code file");
            close(serverSocket);
            return (void*)1;
        }
        fseek(sourceFile, 0, SEEK_END);
        long fileSize = ftell(sourceFile);
        fseek(sourceFile, 0, SEEK_SET);
        fd_set readfds;

        // Set up the file descriptor set
        FD_ZERO(&readfds);
        FD_SET(serverSocket, &readfds);

        char file_size[MAX_FILE_SIZE];
        memcpy(file_size, &fileSize, sizeof(fileSize));
        if(send(serverSocket, &file_size, sizeof(file_size), 0) == -1)
        {
            perror("Error sending file size\n");
            fclose(sourceFile);
            return (void*)-1;
        }

        while ((bytesRead = fread(buffer, 1, sizeof(buffer), sourceFile)) > 0) 
        {
            // int r = send(serverSocket, buffer, bytesRead, 0);
            // printf("counter read %d\n",r);
            if(send(serverSocket, buffer, bytesRead, 0) == -1)
            {
                perror("Error sending file data\n");
                fclose(sourceFile);
                return (void*)-1;
            }
            bzero(buffer, MAX_BUFFER_SIZE);
        }
        // printf("bytesRead: %ld\n", bytesRead);
        // gettimeofday(&Tsend, NULL);
        fclose(sourceFile);

        char response[MAX_BUFFER_SIZE];
        long double responseTime;
        int bytesReceived;

        // Use select with a timeout
        int ready = select(serverSocket + 1, &readfds, NULL, NULL, &timeout);

        if(ready == -1) 
        {
            perror("Error in select");
        } 
        else if (ready == 0) 
        {
            numTimeout++;
            printf("Timeout occurred...\n");
        } 
        else 
        {
            if (FD_ISSET(serverSocket, &readfds)) 
            {
            // Data is available to read
            // Perform your read operations here
                bytesReceived = recv(serverSocket, response, sizeof(response), 0);
                printf("bytesReceived: %d\n", bytesReceived);
            }
        }

        // gettimeofday(&Trecv, NULL);
        // responseTime = (Trecv.tv_sec - Tsend.tv_sec) * 1000000 + (Trecv.tv_usec - Tsend.tv_usec);
        // avgResponseTime += responseTime;
        if (bytesReceived > 0) 
        {
            response[bytesReceived] = '\0';
            numSuccessfulResp++;
            // printf("ResponseTime(in MicroSec) : %Lf\n", responseTime);
            printf("Response : %s\n", response);
        }
        bzero(response, MAX_BUFFER_SIZE);
        bzero(buffer, MAX_BUFFER_SIZE);
        close(serverSocket);
        printf("Loop completed\n");
}

int main(int argc, char* argv[]) 
{
    struct timeval start, end, Tsend, Trecv;
    double avgThroughput;
    double avgTimeout;
    double avgRequestRateSent;
    long double loopCompleteTime = 0.0;

    // int counter;
    if (argc != 9) 
    {
        fprintf(stderr, "Usage: %s <serverIP> <port> <sourceCodeFilePath> <loopNum> <sleepTimeSeconds> <clientid> <numClients> <timeout>\n", argv[0]);
        return 1;
    }

    char* serverAddressStr = argv[1];
    char* serverPortStr = argv[2];
    
    if (!serverAddressStr || !serverPortStr) 
    {
        fprintf(stderr, "Invalid server address format.\n");
        return 1;
    }

    sourceCodeFilePath = argv[3];
    char* numLoop = argv[4];
    char* sleeptime = argv[5];
    char* clientID = argv[6];
    char* numClients = argv[7];
    char* tos = argv[8];

    printf("ClientID: %s, sourceCodeFilePath: %s, numLoop: %s, sleepTime: %s, numClients: %s, tos: %s\n", clientID, sourceCodeFilePath, numLoop, sleeptime, numClients, tos);

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(atoi(serverPortStr));
    if(inet_pton(AF_INET, serverAddressStr, &serverAddress.sin_addr) <= 0) 
    {
        perror("Invalid server address");
        return 1;
    }

    // Set the receive timeout option
    timeout.tv_sec = atoi(tos);
    timeout.tv_usec = 0;

    gettimeofday(&start, NULL);
    for(int i = 0; i < atoi(numLoop); i++)
    {
        pthread_t thread;
        gettimeofday(&Tsend, NULL);
        if (pthread_create(&thread, NULL, createClient, NULL) != 0)
            printf("Failed to create Thread\n");
        pthread_join(thread, NULL);
        gettimeofday(&Trecv, NULL);
        responseTime = (Trecv.tv_sec - Tsend.tv_sec) * 1000000 + (Trecv.tv_usec - Tsend.tv_usec);
        avgResponseTime += responseTime;
        printf("ResponseTime(in MicroSec) : %Lf\n", responseTime);
        sleep(atoi(sleeptime));
    }
    // pthread_exit(NULL);
    gettimeofday(&end, NULL);
    loopCompleteTime = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    printf("loop complete time: %Lf\n", loopCompleteTime);
    avgResponseTime = avgResponseTime / numSuccessfulResp;
    avgThroughput = (numSuccessfulResp / loopCompleteTime) * 1000000;
    avgTimeout = (numTimeout / loopCompleteTime) * 1000000;
    avgRequestRateSent = avgThroughput + avgTimeout;
    printf("avgResponseTime(microsec): %Lf avgThroughput(resps/sec): %f loopCompleteTime(microsec): %Lf numSuccessfulResp: %d avgTimeoutRate(resps/sec): %f avgRequestRateSent(resps/sec): %f\n", avgResponseTime, avgThroughput, loopCompleteTime, numSuccessfulResp, avgTimeout, avgRequestRateSent);
    printf("numTimeout : %d\n", numTimeout);

    char newfile1[100];
    strcpy(newfile1, numClients);
    strcat(newfile1, "_AvgResponseTime.txt");
    FILE *file1 = fopen(newfile1, "a");
    // FILE *file1 = fopen("AvgResponseTime.txt", "a");
    fprintf(file1, "%Lf\n", avgResponseTime);
    printf("AvgResponseTime File created: %s\n", newfile1);

    char newfile2[100];
    strcpy(newfile2, numClients);
    strcat(newfile2, "_AvgThroughput.txt");
    FILE *file2 = fopen(newfile2, "a");
    // FILE *file2 = fopen("AvgThroughput.txt", "a");
    fprintf(file2, "%f\n", avgThroughput);
    printf("AvgThroughput File created: %s\n", newfile2);

    char newfile3[100];
    strcpy(newfile3, numClients);
    strcat(newfile3, "_numTimeouts.txt");
    FILE *file3 = fopen(newfile3, "a");
    // FILE *file2 = fopen("AvgThroughput.txt", "a");
    fprintf(file3, "%d\n", numTimeout);
    printf("numTimeouts File created: %s\n", newfile3);

    char newfile4[100];
    strcpy(newfile4, numClients);
    strcat(newfile4, "_reqSentRate.txt");
    FILE *file4 = fopen(newfile4, "a");
    fprintf(file4, "%f\n", avgRequestRateSent);
    printf("RequestRateSent File created: %s\n", newfile4);

    fclose(file1);
    fclose(file2);
    fclose(file3);
    fclose(file4);
    return 0;
}
