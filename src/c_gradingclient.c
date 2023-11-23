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
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <serverIP:port> <sourceCodeFile> <loopNum> <sleepTimeSeconds>\n", argv[0]);
        return 1;
    }

    char* serverAddressStr = strtok(argv[1], ":");
    char* serverPortStr = strtok(NULL, ":");
    if (!serverAddressStr || !serverPortStr) {
        fprintf(stderr, "Invalid server address format.\n");
        return 1;
    }

    char* sourceCodeFile = argv[2];
	
	//getting the loopNum and sleeptimeseconds into a variable
	unsigned int loopNum = atoi(argv[3]);
	unsigned int sleepTimeSeconds = atoi(argv[4]);

	//variable for response time calculation 
	struct timeval Tsend, Trecv ;
	double responseTime;

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
		printf("Response Time : %lf sec\n", responseTime);
		if (bytesReceived2 > 0) {
			response2[bytesReceived2] = '\0';
			printf("Response2 : %s\n", response2);
		}
		printf("itiration: %d\n", i);
		sleep(sleepTimeSeconds);
	}

	printf("closing socket\n");
	int sen =-1;
	send(serverSocket, &sen, sizeof(sen), 0); 
    close(serverSocket);
    return 0;
}
