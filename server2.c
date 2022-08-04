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
    printf("Creating a socket...\n");

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

        //create fork()
        int pid = fork();
        
        //check if the fork created success
        if(pid < 0){
            error("Failed to fork");
        }

        //the child process
        if(pid == 0){

            // write to client
            char buffer[100];
            memset(buffer,0,100);
            strncpy(buffer,"Hello from server\n",strlen("Hello from server\n"));
        
            int s = write(clientfd,buffer,100);
        
            if(s < 0){
                error("write error");
            }

            printf("Write successful\n");
            

            while(1){

                //read from client
                char incoming[100];
                ssize_t string = read(clientfd, incoming, 100);

                if(string < 0){
                    error("read error");
                }

                printf("Read successful. Received message %s\n", incoming);

                //check command
                if(strlen(incoming) < 4){
                    error("Invalid Commadn\n");
                }

                //if command work
                for(int i = 0; i < 3; i++){
                    char tmp = incoming[i];
                    incoming[i] = toupper(tmp);
                }

                //read command "Get"
                if(strstr(incoming, "GET") != NULL){
                    
                    //check the incoming is longer enough
                    if(strlen(incoming) < 5){

                        memset(incoming, 0, 100);
                        strncpy(incoming, "SERVER 500 Get Error\n", strlen("SERVER 500 Get Error\n"));
                        
                        int strin = write(clientfd, incoming, 100);

                        if(strin < 0){
                            error("write error");
                        }
                    }

                    //Read File Name and save into fileName[100]
                    char fileName[100];
                    memset(fileName, 0, 100);

                    for(int i = 4; i < strlen(incoming) - 1; i++){
                        char tmp = incoming[i];
                        fileName[i-4] = tmp;
                    }

                    // find the file and make it as FILE* in to read
                    FILE *in = fopen(fileName, "r");

                    char fileText[255];

                    // check the fileName is correct, if not
                    // send "SERVER 404 Not Found"
                    if(in == NULL) {

                        printf("SERVER 404 Not Found\n");
                        write(clientfd, "SERVER 404 Not Found\n", strlen("SERVER 404 Not Found\n"));

                    } else {

                        //if file reading pass the text to client
                        write(clientfd, "SERVER 200 OK\n\n", strlen("SERVER 200 OK\n\n"));

                    }

                    while(feof((FILE*)in) == 0){

                        fgets(fileText, 254, (FILE*)in);

                        if(feof((FILE*)in) == 1){
                            write(clientfd, "\n\n\n", strlen("\n\n\n"));
                            break;
                        }
                        
                        write(clientfd, fileText, strlen(fileText));

                    }

                    //cloose the in FILE
                    fclose(in);

                    //reset incoming and fileText array
                    *incoming = '\0';
                    *fileText = '\0';
                    
                } 
                
                //use "bye" to close
                if (strstr(incoming, "BYE") != NULL){ 
                    
                    printf("Closing.....\n");
                    write(clientfd, "Closing.....\n", strlen("Closing.....\n"));

                    exit(0);
                    close(clientfd);
                    return 0;
                }

                if (strstr(incoming, "PUT") != NULL){ 
                    
                    //check the incoming is longer enough
                    if(strlen(incoming) < 5){

                        memset(incoming, 0, 100);
                        strncpy(incoming, "SERVER 502 Command Error\n", strlen("SERVER 502 Command Error\n"));

                        if(incoming < 0){
                            error("SERVER 502 Command Error\n");
                        }
                    }


                    //Read File Name and save into fileName[100]
                    char fileName[100];
                    memset(fileName, 0, 100);

                    for(int i = 4; i < strlen(incoming) - 1; i++){
                        char tmp = incoming[i];
                        fileName[i-4] = tmp;
                    }

                    //check the fileName is correct
                    char putText[255];
                    char newText[255];

                    //FILE *putIn = fopen(fileName, "r");
                    FILE *putOut = fopen(fileName,"rw");

                    // If failure to open file
                    // send "SERVER 501 Put Error"
                    if(putOut == NULL) {
                        printf("SERVER 501 Put Error\n");
                    }


                    if(putOut != NULL){

                        int count = 0;

                        while(count != 2){

                            //if file in, read putText
                        
                            write(clientfd, ("Now you in %s , please put Text in:\n",fileName), strlen(("Now you in %s , please put Text in:\n",fileName)));

                            scanf("%s", putText);

                            fwrite(putText, 1, sizeof(putText), (FILE*)putOut);

                            fgets(newText, 255, (FILE*)putOut);

                            write(clientfd, newText, strlen(newText));

                            if( *incoming = '\n'){

                                count++;
                            }
                            //two new lines break out loop

                        }
                        
                        //close in file
                        fclose(putOut);

                    }

                    *incoming = '\0';
                
                }

            }
        }    

    }
   
    close(sockfd);
    return 0;

}

