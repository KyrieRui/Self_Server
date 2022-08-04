/**
 * Skeleton file for server.c
 * 
 * You are free to modify this file to implement the server specifications
 * as detailed in Assignment 3 handout.
 * 
 * As a matter of good programming habit, you should break up your imple-
 * mentation into functions. All these functions should contained in this
 * file as you are only allowed to submit this file.
 */ 

#include <stdio.h>
#include <ctype.h>  //toupper()
#include <string.h> //copy string 
#include <stdlib.h> //exit()
#include <unistd.h> //read and write

#include <sys/socket.h> //socket
#include <netinet/in.h> // socket address struct sockaddr_in 

// Include necessary header files

//print error message
void error(const char *msg){

    printf("Error: %s\n", msg);
    exit(1);
}

/**
 * The main function should be able to accept a command-line argument
 * argv[0]: program name
 * argv[1]: port number
 * 
 * Read the assignment handout for more details about the server program
 * design specifications.
 */ 
int main(int argc, char *argv[])
{

    int SERVER_PORT = 0;
    int i;
    int num;

    for( i = 1; i < argc; i++ ){
        char *c = argv[i];
        sscanf( c, "%d", &num );
        if(SERVER_PORT < 10){
            SERVER_PORT += num;
        }else{
            SERVER_PORT += num * i * 10;
        }
        
    }


    if(SERVER_PORT < 1024){
        error("Port can't less than 1024");
        return -1;
    }

    printf("Address is: %d\n",SERVER_PORT);


    // Step 1 - create a socket
    int sockfd;
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0){
        error("Error creating socket");
    }
    printf("Socket Created\n");
    

    //Step 2 - bind socket to an address
    //define IP and Port 
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr= INADDR_ANY;
    serveraddr.sin_port = htons(SERVER_PORT);
    
    printf("Address created\n");

    //bind address to socket
    int br;
    br = bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if( br < 0 ){
         error("Error Binding");
    }
    printf("Bind successful\n");

    //Step 3 - listen for connection requests
    if(listen(sockfd, 5) < 0){
        error("Litening error");
    }

    printf("Success listening\n");
    

    while(1){
    
        //Step 4 - accept connection requests from clients
        //client address for communication is included in the accept system call
            
        struct sockaddr_in clientaddr;
        int clienlen = sizeof(clientaddr);
            
        int clientfd = accept(sockfd,(struct sockaddr*)&clientaddr,(socklen_t*)&clientaddr);
    
        if(clientfd<0){
            error("Error accepting client");
        }
        
        printf("Client connection accepted\n");
    
        // write to client
        char buffer[100];
    
        memset(buffer,0,100);
        strncpy(buffer,"Hello from server\n",strlen("Hello from server\n"));
    
        int s = write(clientfd,buffer,100);
    
        if(s < 0){
            error("write error");
        }

        printf("Write successful\n");
        
        //read from client
        char incoming[100];

        memset(incoming, 0, 100);

        ssize_t string = read(clientfd, incoming, 100);

        if(string < 0){
            error("read error");
        }

        printf("Read successful\n");

        //check read
        if(strlen(incoming) < 4){
            error("Invalid Command\n");
        }

        //if command work
        for(int i = 0; i < 3; i++){
            char tmp = incoming[i];
            incoming[i] = toupper(tmp);
        }

        //if "bye"
        if(strncmp(incoming, "BYE", 3) == 0){
            if(listen(sockfd, 5)){
                error("Listening error");
            }
            printf("Listening\n");
        }

        //if "get"
        if(strncmp(incoming, "GET", 3) == 0){

            if(strlen(incoming) < 5){

                memset(incoming, 0, 100);
                strncpy(incoming, "SERVER 500 Get Error\n", strlen("SERVER 500 Get Error\n"));
                
                int strin = write(clientfd, incoming, 100);

                if(strin < 0){
                    error("write error");
                }
            }

            //read file name
            char fileName[100];
            memset(fileName, 0, 100);
            for(int i = 4; i < strlen(incoming) - 1; i++){
                char tmp = incoming[i];
                fileName[i-4] = tmp;
            }

            printf("%s", fileName);
            char *fileText = NULL;
            FILE *in = fopen(fileName, "r");

            if(in != NULL){


                if(fseek(in, 0L, SEEK_END) == 0){
                    long fileSize = ftell(in);
                    if(fileSize == 1){
                        error("File Size Error");
                    }

                    fileText = malloc(sizeof(char) * (fileSize + 1));

                    if(fseek(in, 0L, SEEK_SET) != 0){
                        error("File start error");
                    }

                    //read file into memory
                    size_t newLen = fread(fileText, sizeof(char), fileSize, in);

                    if(ferror(in) != 0){

                        fputs("Error reading file", stderr);

                    } else {

                        fileText[newLen++] = '\0';

                    }

                }

                fclose(in);
                printf("%s, fileText");
            }

        }

    }
   
    close(sockfd);
    return 0;

}

