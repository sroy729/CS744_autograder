#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <limits.h>

#define MAX_BUFFER_SIZE 1024
#define MAX_FILE_SIZE 4

typedef struct __myarg
{
    int skfd;
    int num;
}myarg;

struct Task
{
    int skfd;
    int id;
};

const char SUB_DIR[] = "./submissions/";
const char EXE_DIR[] = "./executables/";
const char OUT_DIR[] = "./outputs/";
const char COMPILER_ERROR_DIR[] = "./compiler_error/";
const char RUNTIME_ERROR_DIR[] = "./runtime_error/";
const char EXPECTED_OUT_DIR[] = "./expected_output/";

pthread_mutex_t mutexQueue;
pthread_cond_t conditionQueue;
int taskCount = 0;
struct TaskQueue* taskqueue;

// A structure to represent a queue
struct TaskQueue {
    int front, rear, size;
    unsigned capacity;
    int* id_array;
    int* fd_array;
};
 
// function to create a queue
// of given capacity.
// It initializes size of queue as 0
struct TaskQueue* createQueue(unsigned capacity)
{
    printf("inside create queue\n");
    struct TaskQueue* queue = (struct TaskQueue*)malloc(
        sizeof(struct TaskQueue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
 
    // This is important, see the enqueue
    queue->rear = capacity - 1;
    queue->id_array = (int*)malloc(queue->capacity * sizeof(int));
    queue->fd_array = (int*)malloc(queue->capacity * sizeof(int));
    return queue;
}
 
// Queue is full when size becomes
// equal to the capacity
int isFull(struct TaskQueue* queue)
{
    return (queue->size == queue->capacity);
}
 
// Queue is empty when size is 0
int isEmpty(struct TaskQueue* queue)
{
    return (queue->size == 0);
}
 
// Function to add an item to the queue.
// It changes rear and size
void enqueue(struct TaskQueue* queue, struct Task task)
{
    printf("inside queue\n");
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1)
                  % queue->capacity;
    queue->id_array[queue->rear] = task.id;
    queue->fd_array[queue->rear] = task.skfd;
    queue->size = queue->size + 1;
    printf("%d enqueued to queue\n", task.id);
}
 
// Function to remove an item from queue.
// It changes front and size
struct Task dequeue(struct TaskQueue* queue)
{
    if (isEmpty(queue));
        // return;
    struct Task t;
    t.id = queue->id_array[queue->front];
    t.skfd = queue->fd_array[queue->front];
    // int item = queue->array[queue->front];
    queue->front = (queue->front + 1)
                   % queue->capacity;
    queue->size = queue->size - 1;
    // return item;
    return t;
}
 
// Function to get front of queue
struct Task front(struct TaskQueue* queue)
{
    if (isEmpty(queue));
        // return;
    struct Task t;
    t.id = queue->id_array[queue->front];
    t.skfd = queue->fd_array[queue->front];
    // return queue->array[queue->front];
    return t;
}
 
// Function to get rear of queue
struct Task rear(struct TaskQueue* queue)
{
    if (isEmpty(queue));
        // return;
    struct Task t;
    t.id = queue->id_array[queue->rear];
    t.skfd = queue->fd_array[queue->rear];
    // return queue->array[queue->rear];
    return t;
}

// Function to compile and run the code
int compileAndRun(const char* sourceFile, int num_clients) 
{
    printf("inside compile and run\n");
    int fd, fd1, fd2;
    int n1, n2, ret;

    char c1[10] = "";
    char c2[10] = "";
    char exe[100] = "";
    char num[10] = "";
    // char charNumber[10];
    sprintf(num, "%d", num_clients);
    strcpy(exe, EXE_DIR);
    strcat(exe, "executable_");
    strcat(exe, num);
    // strcat(exe, ".c");
    char comp[100] = "";
    strcpy(comp, COMPILER_ERROR_DIR);
    strcat(comp, "compileError_");
    strcat(comp, num);
    strcat(comp, ".txt");

    char runt[100] = "";
    strcpy(runt, RUNTIME_ERROR_DIR);
    strcat(runt, "runtimeError_");
    strcat(runt, num);
    strcat(runt, ".txt");

    char out[100] = "";
    strcpy(out, OUT_DIR);
    strcat(out, "output_");
    strcat(out, num);
    strcat(out, ".txt");

    char expout[100] = "";
    strcpy(expout, EXPECTED_OUT_DIR);
    strcat(expout, "out_");
    strcat(expout, num);
    strcat(expout, ".txt");

    char compile_command[200] = "";
    strcat(compile_command, "gcc ");
    strcat(compile_command, sourceFile);
    strcat(compile_command, " -o ");
    strcat(compile_command, exe);
    strcat(compile_command, " 2> ");
    strcat(compile_command, comp);
    printf("Compile_command: %s\n", compile_command);

    char run_command[200] = "";
    strcat(run_command, "./");
    strcat(run_command, exe);
    strcat(run_command, " > ");
    strcat(run_command, runt);
    printf("Run_command: %s\n", run_command);

    char wrapper_command[200] = "";
    strcat(wrapper_command, "./wrapper ./");
    strcat(wrapper_command, exe);
    // printf("wrapper_command: %s\n", wrapper_command);

    // int compileResult = system("gcc program.c -o executable 2> compileError.txt");
    int compileResult = system(compile_command);
    printf("compileResult:%d\n", compileResult);

    if (compileResult != 0) {
        // fprintf(stderr, "COMPILER ERROR\n");
        printf("compile return\n");
        return 0;
    }

    // int executionResult = system("./executable > runtimeError.txt");
    int executionResult = system(run_command);
    if (executionResult != 0) 
    {
        // fprintf(stderr, "RUNTIME ERROR\n");
        system("g++ wrapper.cpp -o wrapper");
        // system("./wrapper ./executable");
        system(wrapper_command);
        return 1;
    }
    else if(executionResult == 0)
    {
        // fd1 = open("runtimeError.txt", O_RDWR, 0644);
        fd1 = open(runt, O_RDWR, 0644);
        // fd = open("output.txt", O_CREAT | O_RDWR, 0644);
        fd2 = open("./expected_output/out.txt", O_RDWR, 0644);
        fd = open(out, O_CREAT | O_RDWR, 0644);
        // fd2 = open(expout, O_RDWR, 0644);
	    if (fd1 < 0 || fd < 0 || fd2 < 0) 
        {
	        exit(1);
	    }
        // int count = 0;
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
            // count++;
        }
        close(fd);
    }
    printf("ret: %d\n", ret);
    return ret;
}

void start_function(struct Task *m) 
{
    // myarg *m = (myarg *) arg;
    int clientSocket = m->skfd;
    int num_clients = m->id;
    int fd1, result;
    char num[10] = "";
    sprintf(num, "%d", num_clients);
    char buffer[MAX_BUFFER_SIZE];
    char runtimebuff[MAX_BUFFER_SIZE];
    char compilebuff[MAX_BUFFER_SIZE];

    char src[100];
    strcpy(src, SUB_DIR);
    strcat(src, "program_");
    strcat(src, num);
    strcat(src, ".c");

    // FILE* sourceFile = fopen("program.c", "wb");
    FILE* sourceFile = fopen(src, "wb");
    if(!sourceFile)
    {
        perror("Error opening file\n");
        // return 0;
    }
    char file_size[MAX_FILE_SIZE];
    int fs = recv(clientSocket, file_size, sizeof(file_size), 0);
    if (fs <= 0) {
        close(clientSocket);
        // break;
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
            // return 0;
        }
        int dd = fwrite(buffer, 1, bytes_recvd, sourceFile);
        printf("buffer fwrite: %d\n", dd);
        bzero(buffer, MAX_BUFFER_SIZE);
        if(bytesRead >= fileSize)
        {
        	break;
        }
    }
    fclose(sourceFile);
    // result = compileAndRun("program.c");
    result = compileAndRun(src, num_clients);
    printf("result: %d\n", result);

    if (result == 0) 
    {
        char msg[MAX_BUFFER_SIZE] = "COMPILE ERROR";
        char comp[100] = "";
        strcpy(comp, COMPILER_ERROR_DIR);
        strcat(comp, "compileError_");
        strcat(comp, num);
        strcat(comp, ".txt");
        // fd1 = open("compileError.txt", O_RDONLY, 0644);
        fd1 = open(comp, O_RDONLY, 0644);
        read(fd1, compilebuff, sizeof(compilebuff));
        strcat(msg, compilebuff);
        printf("compile time error value: %s\n", msg);
        send(clientSocket, msg, sizeof(msg), 0); 
        bzero(msg, MAX_BUFFER_SIZE);
        bzero(compilebuff, MAX_BUFFER_SIZE);
    }
    else if (result == 1)
    {
        char msg[MAX_BUFFER_SIZE] = "RUNTIME ERROR";
        char run[100] = "";
        strcpy(run, RUNTIME_ERROR_DIR);
        strcat(run, "runtimeError_");
        strcat(run, num);
        strcat(run, ".txt");
        // fd1 = open("runError.txt", O_RDONLY, 0644);
        fd1 = open(run, O_RDONLY, 0644);
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
        char out[100] = "";
        strcpy(out, OUT_DIR);
        strcat(out, "output_");
        strcat(out, num);
        strcat(out, ".txt");
        // fd1 = open("output.txt", O_RDONLY, 0644);
        fd1 = open(out, O_RDONLY, 0644);
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
        char run[100] = "";
        strcpy(run, RUNTIME_ERROR_DIR);
        strcat(run, "runtimeError_");
        strcat(run, num);
        strcat(run, ".txt");
        // fd1 = open("runtimeError.txt", O_RDONLY, 0644);
        fd1 = open(run, O_RDONLY, 0644);
        read(fd1, runtimebuff, sizeof(runtimebuff));
        strcat(msg, runtimebuff);
        printf("Fail output value: %s", msg);
        send(clientSocket, msg, sizeof(msg), 0);
        printf("\nOUTPUT ERROR\n");
        bzero(msg, MAX_BUFFER_SIZE);
        bzero(runtimebuff, MAX_BUFFER_SIZE);
    }
    // }
    close(clientSocket);  
    printf("Client completed\n");  
}

void submitTask(struct Task t) 
{
    printf("inside submit task\n");
    pthread_mutex_lock(&mutexQueue);
    enqueue(taskqueue, t);
    taskCount++;
    pthread_mutex_unlock(&mutexQueue);
    pthread_cond_signal(&conditionQueue);
}

void* thread_function(void* args) 
{
    printf("inside thread function\n");
    while(1) 
    {
        struct Task task;

        pthread_mutex_lock(&mutexQueue);
        while (taskCount == 0) 
        {
            pthread_cond_wait(&conditionQueue, &mutexQueue);
        }
        task = dequeue(taskqueue);
        taskCount--;
        pthread_mutex_unlock(&mutexQueue);
        start_function(&task);
    }
}

int main(int argc, char* argv[]) 
{
    int fd1, result, numThread;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    numThread = atoi(argv[2]);

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

    if (mkdir(SUB_DIR, 0777) == 0)
        { printf("Directory '%s' created successfully.\n", SUB_DIR); } 
    else 
        { perror("Error creating directory"); }
    if (mkdir(EXE_DIR, 0777) == 0)
        { printf("Directory '%s' created successfully.\n", EXE_DIR); } 
    else 
        { perror("Error creating directory"); }
    if (mkdir(OUT_DIR, 0777) == 0)
        { printf("Directory '%s' created successfully.\n", OUT_DIR); } 
    else 
        { perror("Error creating directory"); }
    if (mkdir(COMPILER_ERROR_DIR, 0777) == 0)
        { printf("Directory '%s' created successfully.\n", COMPILER_ERROR_DIR); } 
    else 
        { perror("Error creating directory"); }
    if (mkdir(RUNTIME_ERROR_DIR, 0777) == 0)
        { printf("Directory '%s' created successfully.\n", RUNTIME_ERROR_DIR); } 
    else 
        { perror("Error creating directory"); }
    if (mkdir(EXPECTED_OUT_DIR, 0777) == 0)
        { printf("Directory '%s' created successfully.\n", EXPECTED_OUT_DIR); } 
    else 
        { perror("Error creating directory"); }

    taskqueue = createQueue(1000);
    printf("taskqueue created\n");

    pthread_t thread[numThread];
    pthread_mutex_init(&mutexQueue, NULL);
    pthread_cond_init(&conditionQueue, NULL);
    printf("inside mutex\n");
    int i;
    for (i = 0; i < numThread; i++) 
    {
        printf("thread create loop\n");
        if (pthread_create(&thread[i], NULL, &thread_function, NULL) != 0) {
            perror("Failed to create the thread");
        }
    }
    
    int clientnum = 0;
    while (1) 
    {
        printf("inside main while loop\n");
        // Accept a client connection
        int clientSocket = accept(serverSocket,  (struct sockaddr *)&clientAddr, &addrLen);
        if (clientSocket == -1) {
            perror("Accept failed");
            continue; // Continue to the next connection
        }
        
        struct Task t;
        t.skfd = clientSocket;
        t.id = clientnum;
        printf("before task submitted\n");
        submitTask(t);
        printf("task submitted\n");
        printf("Accepted connection from %d %d\n", ntohs(clientAddr.sin_port), clientnum);
        clientnum++;
    }

    for (i = 0; i < numThread; i++) {
        if (pthread_join(thread[i], NULL) != 0) {
            perror("Failed to join the thread");
        }
    }
    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&conditionQueue);
    close(serverSocket);
    return 0;
}