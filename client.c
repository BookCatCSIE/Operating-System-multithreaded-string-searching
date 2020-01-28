#include "client.h"

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include<pthread.h>

#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>

char temp_message[1000];
int i = 0;

void * cientThread(void *arg)
{
    //printf("In thread\n");
    char message[1000];
    char buffer[1024];
    int clientSocket;
    struct sockaddr_in serverAddr;
    //socklen_t addr_size;
    // Create the socket.
    //clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serverAddr, '0', sizeof(serverAddr));
    //Configure settings of the server address
// Address family is Internet
    serverAddr.sin_family = AF_INET;
    //Set port number, using htons function
    serverAddr.sin_port = htons(8080);//7799
//Set IP address to localhost
    //serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    //memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    if(inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    //Connect the socket to the server using the address
    //addr_size = sizeof serverAddr;
    //connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    strcpy(message,temp_message);
    //fgets(message,1000,stdin);

    if( send(clientSocket, message, strlen(message), 0) < 0)
    {
        printf("Send failed\n");
    }
    //Read the message from the server into the buffer
    if(recv(clientSocket, buffer, 1024, 0) < 0)
    {
        printf("Receive failed\n");
    }
    //Print the received message
    //printf("Data received: %s\n",buffer);
    printf("%s\n",buffer);
    i--;
    close(clientSocket);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    /*  single thread
    while(1){
        fgets(temp_message,1000,stdin);

        pthread_t tid;
        if( pthread_create(&tid, NULL, cientThread, NULL) != 0 ){
             printf("Failed to create thread\n");
        }

        pthread_join(tid,NULL);
    }
    */

    //-------------------------------------------------------------------

    //multi-thread
    //int i = 0;
    pthread_t tid[50];
    while(i<50)
    {
        fgets(temp_message,1000,stdin);

        if( pthread_create(&tid[i], NULL, cientThread, NULL) != 0 )
        {
            printf("Failed to create thread\n");
        }
        pthread_join(tid[i],NULL);
        i++;
    }

    /*
    //sleep(20);
    i = 0;
    while(i< 50)
    {
       pthread_join(tid[i++],NULL);
       printf("%d:\n",i);
    }
    */


    return 0;
}
