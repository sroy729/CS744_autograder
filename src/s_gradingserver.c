#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MAX_BUFFER_SIZE 1024

//docs:: correctly recieve the full file 
//bascically to recieve a file of any size
int recv_file(int sockfd, char*file_path){
	char buffer[MAX_BUFFER_SIZE];
	bzero(buffer, MAX_BUFFER_SIZE);
	int file = open(file_path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

	printf("file des :: %d \n", file);
	if (file <0){
		perror("Error in opening the file");
		return -1;
	}

	// char file_size_bytes[MAX_BUFFER_SIZE];
	int file_size;
	if (recv(sockfd, &file_size, sizeof(file_size), 0) == -1){
		perror("Error in recieving the file size");
		close(file);
		return -1;
	}

	// memcpy(&file_size, file_size_bytes, sizeof(file_size_bytes));

	printf("[Debug] file size is :: %d\n", file_size);
	if (file_size == -1){
		close(file);
		return -2;
	}
	// printf("file des :: %d \n", file);
	
	size_t byte_read = 0, total_byte_read =0;
	while(true){
		//read buffer amount of data
		printf("ssinnnnn :: %d\n ", sizeof(buffer));
		byte_read = recv(sockfd, buffer, sizeof(buffer), 0);
		total_byte_read += byte_read;
		if(byte_read <= 0){
			perror("error in reciving the file");
			close(file);
			return -1;
		}
		printf("[DEBUG] buffer:%ld  \n", byte_read);
		int r = write(file, buffer, byte_read);
		printf("return %d\n", r);
		//write the buffer into the file
		//fwrite(buffer, 1, byte_read, file); 
		//printf("[DEBUG] buffer:  \n", buffer);

		bzero(buffer, MAX_BUFFER_SIZE);
		if(total_byte_read >= file_size){
			break;
		}
	}
	close(file);
	printf("jhunag\n");
	return 0;
}

//docs:: Function to compile and run the code
int compileAndRun(const char* sourceFile) {
    int fd, fd1, fd2;
    int n1, n2, ret;

    char c1[10];
    char c2[10];
	//docs:: compiling the program and capturing the error if there is any
    int compileResult = system("g++ output/s_program.cpp -o bin/executable 2> output/compileError.txt");
    // printf("compileResult:%d\n", compileResult);

    if (compileResult != 0) {
        // fprintf(stderr, "COMPILER ERROR\n");
        // printf("compile return\n");
        return 0; //exit the funtion as soon as we get compile error
    }

	//docs:: try to run can capture the error
    int executionResult = system("bin/executable > output/runtimeError.txt");

    if (executionResult != 0) 
    {
        // fprintf(stderr, "RUNTIME ERROR\n");
		//docs:: different from conventional 
        system("g++ src/wrapper.cpp -o bin/wrapper");
        system("bin/wrapper bin/executable");
        return 1;
    }
    else if(executionResult == 0)
    {
        fd1 = open("output/runtimeError.txt", O_RDWR, 0644);
        fd = open("output/output.txt", O_CREAT | O_RDWR, 0644);
        fd2 = open("output/out.txt", O_RDWR, 0644);
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

    int fd, fd1, result;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Binding failed");
        return 1;
    }

    listen(serverSocket, 5);

accept:
	int clientSocket = accept(serverSocket, NULL, NULL);
    while (1) {
        if (clientSocket == -1) {
            perror("Accept failed");
            continue;
        }
        char buffer[MAX_BUFFER_SIZE];
        char runtimebuff[MAX_BUFFER_SIZE];
        char compilebuff[MAX_BUFFER_SIZE];
		char tempSendbuff[MAX_BUFFER_SIZE];
        char *resultbuff;
		// the function first recieve the file size and then the file
		int ret = recv_file(clientSocket, "output/s_program.cpp") ;
		if(ret  != 0){
			close(clientSocket);
			if (ret == -2){
				printf("recieved -1\n");
				goto accept;
			}
			return 0;
			continue;
		}

        result = compileAndRun("output/s_program.cpp");

        if (result == 0) {
            // printf("\nCOMPILE ERROR\n");
			strcpy(tempSendbuff, "COMPILE ERROR\n");
            send(clientSocket, tempSendbuff, MAX_BUFFER_SIZE, 0);
            // resultbuff = "\nCOMPILE ERROR\n";
            // fd = open("compileError.txt", O_RDWR | O_APPEND, 0644);
            // printf("write: %ld\n", write(fd, resultbuff, sizeof(resultbuff)));
            // close(fd);
            fd1 = open("output/compileError.txt", O_RDONLY, 0644);
            read(fd1, buffer, sizeof(buffer));
            printf("compile time error value: %s\n", buffer);
            send(clientSocket, buffer, sizeof(buffer), 0);

        }
        else if (result == 1) {
            // printf("\nRUNTIME ERROR\n");
			strcpy(tempSendbuff, "RUNTIME ERROR\n");
            send(clientSocket, tempSendbuff, MAX_BUFFER_SIZE, 0);
            // resultbuff = "\nRUNTIME ERROR\n";
            // fd = open("runError.txt", O_RDWR | O_APPEND, 0644);
            // printf("write: %ld\n", write(fd, resultbuff, sizeof(resultbuff)));
            // close(fd);
            fd1 = open("output/runError.txt", O_RDONLY, 0644);
            read(fd1, runtimebuff, sizeof(runtimebuff));
            printf("runtime error value: %s\n", runtimebuff);
            send(clientSocket, runtimebuff, sizeof(runtimebuff), 0);
        }
        else if (result == 2) {
			memset(tempSendbuff, 0, MAX_BUFFER_SIZE); 
			strcpy(tempSendbuff, "PASS\n");
            int bs = send(clientSocket, tempSendbuff, MAX_BUFFER_SIZE, 0);
			printf("[Debug] bs: %d\n" , bs);
            // resultbuff = "\nPASS\n";
            // fd = open("output.txt", O_RDWR | O_APPEND, 0644);
            // printf("write: %ld\n", write(fd, resultbuff, sizeof(resultbuff)));
            // close(fd);
            fd1 = open("output/output.txt", O_RDONLY, 0644);
            read(fd1, runtimebuff, sizeof(runtimebuff));
            printf("pass output value: %s\n", runtimebuff);
            int bs2 = send(clientSocket, runtimebuff, sizeof(runtimebuff), 0);
			printf("[Debug] bs2: %d\n" , bs2);

            // send(clientSocket, "PASS", 4, 0);
            printf("\nPASS\n");
        }
        else if (result == 3){ 
			strcpy(tempSendbuff, "OUTPUT ERRIR\n");
            send(clientSocket, tempSendbuff, MAX_BUFFER_SIZE, 0);
            // resultbuff = "\nOUTPUT ERROR\n";
            // fd = open("runtimeError.txt", O_RDWR | O_APPEND, 0644);
            // printf("write: %ld\n", write(fd, resultbuff, sizeof(resultbuff)));
            // close(fd);
            
            fd1 = open("output/runtimeError.txt", O_RDONLY, 0644);
            read(fd1, runtimebuff, sizeof(runtimebuff));
            printf("fail output value: %s\n", runtimebuff);
            send(clientSocket, runtimebuff, sizeof(runtimebuff), 0);
            printf("\nOUTPUT ERROR\n");
        }
        //close(clientSocket);
    }
	printf("closing\n");

    close(serverSocket);
    return 0;
}
