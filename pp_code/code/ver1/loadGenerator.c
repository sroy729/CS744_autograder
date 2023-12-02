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

#define MAX_BUFFER_SIZE 1024
#define MAX_FILE_SIZE 4

int main(int argc, char* argv[]) 
{
    // struct timeval tv;
    struct timeval start, end, Tsend, Trecv; 
    // struct timezone tz;
    // long double start, end, Tsend, Trecv;
    long double avgResponseTime = 0.0;
    double avgThroughput;
    long double loopCompleteTime = 0.0;
    int numSuccessfulResp = 0;
    // int counter;
    if (argc != 8) 
    {
        fprintf(stderr, "Usage: %s <serverIP> <port> <sourceCodeFilePath> <loopNum> <sleepTimeSeconds> <clientid> <numClients>\n", argv[0]);
        return 1;
    }

    // char* serverAddressStr = strtok(argv[1], ":");
    // char* serverPortStr = strtok(NULL, ":");
    char* serverAddressStr = argv[1];
    char* serverPortStr = argv[2];
    
    if (!serverAddressStr || !serverPortStr) 
    {
        fprintf(stderr, "Invalid server address format.\n");
        return 1;
    }

    char* sourceCodeFilePath = argv[3];
    char* numLoop = argv[4];
    char* sleeptime = argv[5];
    char* clientID = argv[6];
    char* numClients = argv[7];

    printf("ClientID: %s, sourceCodeFilePath: %s, numLoop: %s, sleepTime: %s, numClients: %s\n", clientID, sourceCodeFilePath, numLoop, sleeptime, numClients);

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) 
    {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in serverAddress;
    bzero((char *)&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(atoi(serverPortStr));
    if(inet_pton(AF_INET, serverAddressStr, &serverAddress.sin_addr) <= 0) 
    {
        perror("Invalid server address");
        return -1;
    }

    int tries = 0;
    while(1)
    {
         if(connect(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == 0) 
            break;
        sleep(2);
        tries += 1;
        if(tries == 3)
        {
            perror("Connection failed");
            return 1;
        }
    }
    // if(connect(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
    // {
    //     perror("Connection failed");
    //     return -1;
    // }

    for(int i = 0; i < atoi(numLoop); i++)
    {
        // gettimeofday(&tv, &tz);
        // start = (long double)tv.tv_usec;
        gettimeofday(&start, NULL);
        // printf("loop start time: %Lf\n", start);
        char buffer[MAX_BUFFER_SIZE];
        size_t bytesRead;
        FILE* sourceFile = fopen(sourceCodeFilePath, "rb");
        if (sourceFile == NULL) 
        {
            perror("Failed to open source code file");
            return -1;
        }
        fseek(sourceFile, 0, SEEK_END);
        long fileSize = ftell(sourceFile);
        fseek(sourceFile, 0, SEEK_SET);

        char file_size[MAX_FILE_SIZE];
        memcpy(file_size, &fileSize, sizeof(fileSize));
        if(send(serverSocket, &file_size, sizeof(file_size), 0) == -1)
        {
            perror("Error sending file size\n");
            fclose(sourceFile);
            return -1;
        }
        while((bytesRead = fread(buffer, 1, sizeof(buffer), sourceFile)) > 0)
        {
            // printf("bytesRead: %ld\n", bytesRead);
            if(send(serverSocket, buffer, bytesRead, 0) == -1)
            {
                perror("Error sending file data\n");
                fclose(sourceFile);
                return -1;
            }
            bzero(buffer, MAX_BUFFER_SIZE);
        }        
        // gettimeofday(&tv, NULL);
        // Tsend = (long double)tv.tv_usec;
        gettimeofday(&Tsend, NULL);

        fclose(sourceFile);

        char response[MAX_BUFFER_SIZE];
        long double responseTime;
        int bytesReceived = recv(serverSocket, response, sizeof(response), 0);
        // printf("bytesReceived: %d\n", bytesReceived);
        // gettimeofday(&tv, &tz);
        // Trecv = (long double)tv.tv_usec;
        gettimeofday(&Trecv, NULL);
        // responseTime = Trecv - Tsend;
        responseTime = (Trecv.tv_sec - Tsend.tv_sec) * 1000000 + (Trecv.tv_usec - Tsend.tv_usec);
        avgResponseTime += responseTime;
        if (bytesReceived > 0) 
        {
            response[bytesReceived] = '\0';
            numSuccessfulResp++;
            printf("ResponseTime(in MicroSec) : %Lf\n", responseTime);
            printf("Response : %s\n", response);
        }
        bzero(response, MAX_BUFFER_SIZE);
        bzero(buffer, MAX_BUFFER_SIZE);
        // printf("Loop completed\n");
        // gettimeofday(&tv, &tz);
        // end = (long double)tv.tv_usec;
        gettimeofday(&end, NULL);
        // printf("loop end time: %Lf\n", end);
        // loopCompleteTime = loopCompleteTime + (end - start);
        loopCompleteTime = loopCompleteTime + (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
        printf("loop complete time: %Lf\n", loopCompleteTime);
        sleep(atoi(sleeptime));
    }
    close(serverSocket);
    avgResponseTime = avgResponseTime / atoi(numLoop);
    avgThroughput = (numSuccessfulResp / loopCompleteTime) * 1000000;
    printf("avgResponseTime(nanosec): %Lf avgThroughput(resps/sec): %f loopCompleteTime(nanosec): %Lf numSuccessfulResp: %d\n", avgResponseTime, avgThroughput, loopCompleteTime, numSuccessfulResp);

    char newfile1[100];
    strcpy(newfile1, numClients);
    strcat(newfile1, "_AvgResponseTime.txt");
    FILE *file1 = fopen(newfile1, "a");
    fprintf(file1, "%Lf\n", avgResponseTime);
    printf("AvgResponseTime File created: %s\n", newfile1);

    char newfile2[100];
    strcpy(newfile2, numClients);
    strcat(newfile2, "_AvgThroughput.txt");
    FILE *file2 = fopen(newfile2, "a");
    fprintf(file2, "%f\n", avgThroughput);
    printf("AvgThroughput File created: %s\n", newfile2);

    fclose(file1);
    fclose(file2);
    return 0;
}
