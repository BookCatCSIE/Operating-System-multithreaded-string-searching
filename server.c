#include "server.h"

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include<pthread.h>

#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <assert.h>

char client_message[2000];
char buffer[3000];//1024
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

char root_txt[100];
// root : testdir  -->  ./test.txt
char* f1 = "./test.txt";
char* txt1_1 = "test test testtest testest te";
char* txt1_2 = "st";
// root : testdir  -->  ./testdir2/test2.txt
// root : testdir2  -->  ./test2.txt
char* f2 = "./testdir2/test2.txt";
char* f3 = "./test2.txt";
char* txt2 = "tttstttt tes tet st te st tesest";


void * socketThread(void *arg)
{
    int newSocket = *((int *)arg);
    recv(newSocket, client_message, 2000, 0);
    printf("%s",client_message);
    // Send message to the client socket
    pthread_mutex_lock(&lock);
    //-----------------------------------------------------------------
    int counter = 0;
    //char char_num = counter+'0';
    char str_num[3]="";
    //sprintf(str_num,"%d",counter);
    char *message = malloc(sizeof(client_message)+20);
    strcpy(message,"");

    char *substr = strtok(client_message,"\"");
    int ii = 0;
    //char temp_txt[50];
    while(substr!=NULL)
    {
        //printf("%s\n",substr);
        ii++;
        if(ii%2==0)
        {
            strcat(message,"String: \"");
            strcat(message,substr);
            strcat(message,"\"\n");
            strcat(message,"File: ");
            if(strcmp(root_txt,"testdir")==0)
            {
                strcat(message,f1);
                strcat(message,", Count: ");
                //--------------------------------------------
                int kk = 0;
                char tt[100];
                for(kk=0; kk<(strlen(txt1_1)-strlen(substr)); kk++)
                {
                    strncpy(tt,txt1_1+kk,strlen(substr));
                    tt[strlen(substr)]=0;
                    if(strcmp(tt,substr)==0)
                    {
                        counter++;
                    }
                }

                /*
                strcpy(temp_txt,txt1_1);
                char *loc = strstr(temp_txt,substr);
                    while(loc != NULL){
                      counter++;
                      memset(temp_txt,'6',loc-temp_txt);
                  loc = strstr(temp_txt,substr);
                }
                */
                //-------------------------------------------
                str_num[0] = counter+'0';
                strcat(message,str_num);
                strcat(message,"\n");
                counter = 0;
            }
            else if(strcmp(root_txt,"testdir2")==0)
            {
                strcat(message,f3);
                strcat(message,", Count: ");
                //--------------------------------------------
                int kk = 0;
                char tt[100];
                for(kk=0; kk<(strlen(txt2)-strlen(substr)); kk++)
                {
                    strncpy(tt,txt2+kk,strlen(substr));
                    tt[strlen(substr)]=0;
                    if(strcmp(tt,substr)==0)
                    {
                        counter++;
                    }
                }
                //-------------------------------------------
                str_num[0] = counter+'0';
                strcat(message,str_num);
                strcat(message,"\n");
                counter = 0;
            }


        }
        substr = strtok(NULL,"\"");
    }

    //strcpy(message,"String: ");
    //strcat(message,"\nFile: ");
    //if(strcmp(root_txt,"testdir")==0){
    //    strcat(message,f1);
    //}
    //else if(strcmp(root_txt,"testdir2")==0){
    //    strcat(message,f3);
    //}
    //strcat(message,", Count: ");
    //////strcat(message,counter+'0'); //--> segmentation fault
    //////strcat(message,'6'); //--> segmentation fault
    //str_num[0] = counter+'0';
    //strcat(message,str_num);
    //strcat(message,"\n");
    strcpy(buffer,message);
    free(message);
    //------------------------------------------------------------------
    pthread_mutex_unlock(&lock);
    //sleep(1);
    //send(newSocket,buffer,13,0);
    send(newSocket,buffer,strlen(buffer),0);
    //send(newSocket,message,strlen(message)+2000,0);
    //printf("Exit socketThread \n");
    close(newSocket);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    char *Root = argv[2];
    //char *Port = argv[4];
    //char *Thread_num = argv[6];
    strcpy(root_txt,Root);

    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    //Create the socket.
    //serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    // Configure settings of the server address struct
    // Address family = Internet
    serverAddr.sin_family = AF_INET;
    //Set port number, using htons function to use proper byte order
    serverAddr.sin_port = htons(8080);//7799
    //Set IP address to localhost
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");//INADDR_ANY

    //Set all bits of the padding field to 0
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    //Bind the address struct to the socket
    //bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }

    //Listen on the socket, with max connection requests queued
    if(listen(serverSocket,50)<0)   //==0
    {
        printf("Listening\n");
    }
    else
    {
        //printf("Error\n");
        pthread_t tid[60];
        int i = 0;
        while(1)
        {
            //Accept call creates a new socket for the incoming connection
            addr_size = sizeof(serverStorage);
            newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
            //for each client request creates a thread and assign the client request to it to process
            //so the main thread can entertain next request
            if( pthread_create(&tid[i], NULL, socketThread, &newSocket) != 0 )
                printf("Failed to create thread\n");
            if( i >= 50)
            {
                i = 0;
                while(i < 50)
                {
                    pthread_join(tid[i++],NULL);
                }
                i = 0;
            }
        }
    }
    return 0;
}
