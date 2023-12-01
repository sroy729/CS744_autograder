#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h> // for gettimeofday()

#define MAX_BUFFER_SIZE 1024

int main(int argc, char* argv[]) {

	if (argc != 7) 
    {
        fprintf(stderr, "Usage: %s <serverIP:port> <sourceCodeFile> <loopNum> <sleepTimeSeconds> <clientid> <numClients>\n", argv[0]);
        return 1;
    }

    long double avgResponseTime = 0.0;
    double avgThroughput;
    double loopCompleteTime = 0.0;
    double tempLoopTime = 0.0;
    int numSuccessfulResp = 0;

	//getting all the arguement assigned to variables
    char* serverAddressStr = strtok(argv[1], ":");
    char* serverPortStr = strtok(NULL, ":");
    char* sourceCodeFile = argv[2];
	unsigned int loopNum = atoi(argv[3]);
	unsigned int sleepTimeSeconds = atoi(argv[4]);
	unsigned int clientID = atoi(argv[5]);
	unsigned int numClients = atoi(argv[6]);//maximum number of clients(M) 

	//variable for response time calculation 
	struct timeval Tsend, Trecv ;
	struct timeval start, end;
	double responseTime;

	//ack all the parameter that are running the client
	printf("\n************Working parameters********************\n");
    printf("ClientID: %d\nsourceCodeFilePath: %s\nnumLoop: %d\nsleepTime: %d\nnumClients: %d\n\n", clientID, sourceCodeFile, loopNum, sleepTimeSeconds, numClients);

    if (!serverAddressStr || !serverPortStr) {
        fprintf(stderr, "Invalid server address format.\n");
        return 1;
    }

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

	for(int i = 0; i< loopNum; i++){
		gettimeofday(&start, NULL);
		printf("serverSocket : %d\n", serverSocket);
		FILE* sourceFile = fopen(sourceCodeFile, "rb");
		if (sourceFile == NULL) {
			perror("Failed to open source code file");
			close(serverSocket);
			return 1;
		}

		fseek(sourceFile, 0, SEEK_END);
		int fileSize = ftell(sourceFile);
		fseek(sourceFile, 0, SEEK_SET);

		char buffer[MAX_BUFFER_SIZE];
		size_t bytesRead;
		bzero(buffer, MAX_BUFFER_SIZE);	

		//first send the file size of the file
		if (send(serverSocket, &fileSize, sizeof(fileSize), 0) == -1){
			perror("Error sending file size");
			fclose(sourceFile);
			return -1;
		}
		
		gettimeofday(&Tsend, NULL);
		while (!feof(sourceFile)) {
			bytesRead = fread(buffer, 1, MAX_BUFFER_SIZE, sourceFile);
			size_t byteSent = 0;
			while(byteSent != bytesRead){//ensuring all bytes are sent
				printf("serverSocket : %d\n", serverSocket);
				byteSent += send(serverSocket, buffer+byteSent, bytesRead, 0);
			}
			printf("[debug] buffer : %ld\n", bytesRead);
			bzero(buffer, MAX_BUFFER_SIZE);	
		}

		fclose(sourceFile);

		char response[MAX_BUFFER_SIZE];
		bzero(response, MAX_BUFFER_SIZE);
		int bytesReceived = recv(serverSocket, response, MAX_BUFFER_SIZE, 0);
		printf("[Debug] byteReceieved: %d\n" , bytesReceived);
		if (bytesReceived > 0) {
			response[bytesReceived] = '\0';
			printf("Response : %s\n", response);
		}

		char response2[MAX_BUFFER_SIZE];
		bzero(response2, MAX_BUFFER_SIZE);
		int bytesReceived2 = recv(serverSocket, response2, MAX_BUFFER_SIZE, 0);
		printf("[Debug] byteReceieved2: %d\n" , bytesReceived2);
		gettimeofday(&Trecv, NULL);
		responseTime =((Trecv.tv_sec -Tsend.tv_sec)*1e6 + Trecv.tv_usec - Tsend.tv_usec)/1e6;
		avgResponseTime += responseTime;
		printf("Response Time : %lf sec\n", responseTime);
		if (bytesReceived2 > 0) {
			response2[bytesReceived2] = '\0';
			printf("Response2 : %s\n", response2);
			numSuccessfulResp++;
		}
		printf("itiration: %d\n", i);
		gettimeofday(&end, NULL);
		//get the cumulative loopcompletetime
		tempLoopTime =((end.tv_sec - start.tv_sec)*1e6 + end.tv_usec - start.tv_usec)/1e6;
		loopCompleteTime += tempLoopTime;
		printf("Loop Complete Time+ : %lf sec\n", loopCompleteTime);
		sleep(sleepTimeSeconds);
	}

	printf("closing socket\n");
	int sen =-1;
	send(serverSocket, &sen, sizeof(sen), 0); 
    close(serverSocket);
    avgResponseTime = avgResponseTime / loopNum;
    avgThroughput = (numSuccessfulResp / loopCompleteTime) /* 1000000000*/;
	//printing connetion summary
    printf("\n\n*****************Connection summary **************\navgResponseTime(sec): %Lf\navgThroughput(resps/sec): %f\nTotal loopCompleteTime(sec): %lf \nnumSuccessfulResp: %d\nRequested: %d\n", avgResponseTime, avgThroughput, loopCompleteTime, numSuccessfulResp, loopNum);

	//connetion logging
    char newfile[100];
	sprintf(newfile, "./output/connection_logs/log_of_%d_M.txt", numClients);
    FILE *file = fopen(newfile, "a+");
	if (file == NULL){
		perror("Error");
		printf("error in file creation\n");
	}
    fprintf(file,"%d %Lf %f\n", clientID, avgResponseTime, avgThroughput);
	printf("hi\n");
    printf("Log File created: %s\n", newfile);
    fclose(file);
    return 0;
}
