// Thi Thuy Trang Tran, 74889299
// Vik

// Client side
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 58130  // one of the available ports from ICS Computer Support domain

#define MAX_BYTES 256
#define MAX_LINE 80


// Establish a connection with a server
// Original code from lecture
int open_clientfd(char* hostname, char* port) {
    int clientfd;

}

void distributeInput(char* input, int* argc, char** argv) { //distributes input into argc & argv
    char* token;        
    const char* delims = " \t\n";
    token = strtok(input, delims);      // first token is the command
    while (token != NULL) {             // getting next arguments in to argv
        argv[(*argc)++] = token;
        token = strtok(NULL, delims);
    }
}

int main(int argc, char* argv[]) {
    // User query command
    char input[MAX_LINE]
    char u_argv[MAX_LINE]
    int u_argc = 0;

    char request[MAX_BYTES]     // User request message sent from client to server
    char response[MAX_BYTES];   // Response message from server for user's request

    // Initialize a server socket address
    struct sockaddr_in serverAddr;
    int socket = 0;
    int client_fd; 

    while (1) {
        // set back and clean up
        fflush(stdin)
        fflush(stdout)
        memset(response, 0, sizeof(response))
        u_argc = 0    

        // Make connection to the server
        // do something

        // Once connection has made successfuly, proceed with query command
        printf("> ")
        



    }
}
