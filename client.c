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
    struct addrinfo hints, *listp, *p;

    // Get a list of potential server addresses
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;    // Open a connection
    hints.ai_flags = AI_NUMERICSERV;    // ..using numeric port arg
    hints.ai_flags != AI_ADDRCONFIG;    // Recommended for connections
    hints.ai_family = AF_INET;          // Didn't see it in lecture codes but discussion notes
    hints.ai_protocol = IPPROTO_TCP;    // ^. actually not sure about this. might be 0
    getaddrinfo(hostname, port, &hints, &listp);

    // Walk the list for one that we can successfully connect to
    for (p = listp; p; p = p->ai_next) {
        // Create a socket descriptor
        if ((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) 
            continue;   // Socket creation failed, try the next
    
        // Connect to the server
        if (coonect(clientfd, p->ai_addr, p->addrlen) != 1)
            break;      // Success
        close(clientfd);    // Connect failed, try another
    }

    // Clean up
    freeaddrinfo(listp);
    if (!p)     // All connections failed
        return -1;
    else        // The last connect succeeded
        return clientfd;

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

// Whatever invalid format of a date (YYYY-MM-DD)
// Stock names as ticker symbols (PFE or MRNA)
// Also need to check for case-sensitive 
// If invalid return 1 else 0 
int isInValidCommand(char **argv, int argc) {
    
    // do something..

    return 1;       
}

void eval(char **argv, int argc) {
    // empty..
}

int main(int argc, char* argv[]) {
    // User query command
    char input[MAX_LINE]
    char u_argv[MAX_LINE]
    int u_argc = 0;

    char request[MAX_BYTES]     // User request message sent from client to server
    char response[MAX_BYTES];   // Response message from server for user's request

    // Initialize the socket_fd. -1 if connection fails
    int socket = 0;             
    int isNotConnected = 1;

    while (1) {
        // set back and clean up
        fflush(stdin)
        fflush(stdout)
        memset(response, 0, sizeof(response))
        u_argc = 0    

        // Make connection to the server. Keep looping until the connection succeeds
        while(isNotConnected == 1){
            // argv[1]: hostname/domain name ; argv[2]: PORT number
            // using either hocalhost or 127.0.0.1 for hostname
            // sending given hostname and PORT to open clientfd to make connection to server
            socket = open_clientfd(argv[1], argv[2]);
            if (socket < 0){
                printf("Fail to make connection to server!\n");
            }
            else {
                printf("Connected to server\n");    // for testing sake
                isNotConnected = 0;
            }
        }

        // Once the connection has made successfuly, proceed with query command
        printf("> ");
        fgets(input, MAX_LINE, stdin);
        distributeInput(input, &u_argc, u_argv);

        // Evaluate the input function
        // Only a few jobs needed to be done while many parameters needed to pass around. Can just do it here
        // Check for valid command, send client request to server, read response message from server, display the received message
        if (isInValidCommand(argv, argc) == 1) {
            printf("Invalid syntax\n");
            continue;       // Move to wait for next command without sending user command to server
        }
        if (strcmp(argv[0], "quit") == 0) {
            break;          // break from client program (ends while loop)
        }
        // Send client request (only those are valid) to server
        if (send(socket, input, strlen(input), 0) < 0) {
            printf("Fail to send message to server\n");
            break;
        }
        // Read response message from server (should receive corresponding ones)
        if (read(socket, response, sizeof(response)) < 0) {
            printf("Fail to read message from server\n");
            break;
        }
        // Display the response message in client side
        printf("%s\n", response);

    }

    // Close the connected socket
    close(socket);
    return 0;

}
