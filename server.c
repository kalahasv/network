// Thi Thuy Trang Tran, 74889299
// Vik

// Server side
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <string.h>
#define PORT 58130

#define MAX_BYTES 256
#define MAX_NUMBER_DATE 504     // used for creating data structures to data from csv files
#define MAX_CHAR_DATE 12        // storing date strings
#define MAX_FILENAME 80         // max string for file name
#define MAX_NUMBER_FILE 2

// Global socket file descriptors. Only closes when Ctrl + C is hit (will be handled in interruptHandler)
int serverfd = -1;     
int connectfd = -1;

void distributeInput(char* input, int* argc, char** argv) { // distributes input into argc & argv
    char* token;        
    const char* delims = " \t\n";
    token = strtok(input, delims);      // first token is the command
    while (token != NULL) {             // getting next arguments in to argv
        argv[(*argc)++] = token;
        token = strtok(NULL, delims);
    }
}

// stock price information. Including date and closing price associated with date
struct info {
    char data[MAX_CHAR_DATE];
    double closingPrice;
};

// data structure for two kinds of stocks. One for MRNA and another one for PFE
struct stock {
    char stockName[MAX_FILENAME];
    //int size;   // size of stock == number of dates in csv
    struct info stockInfo[MAX_NUMBER_DATE];
}stockList[MAX_NUMBER_FILE];   // global stock list. Only contains two stocks: MRNA and PFE

void readFromFile();    // read from File and put data into data structure for each stock

void interruptHandler(int signalNum) {  // quit server when Ctrl-C is hit
    printf("Ends the server\n");
    close(serverfd);
    close(connectfd);
    exit(0);    // terminate the process
}     

// Create a listening descriptor that can be used to accept connection requests from clients
// refering to the lecture codes
int open_listenfd(char *port) {
    struct addrinfo hints, *listp, *p;
    int listenfd, optval = 1;

    // Get a list of potential server addresses
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;                // Accept connect..
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;    // ..on any IP address
    hints.ai_flags |= AI_NUMERICSERV;               // ..using port no. 
    getaddrinfo(NULL, port, &hints, &listp);

    // Walk the list for one that we can bind to
    for (p = listp; p; p = p->ai_next) {
        // Create a socket descriptor
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
            continue;   // socket failed, try the next one
        }

        // Eliminates "Address already in use" error from bind
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

        // Bind the descriptor to the address
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) {
            break;  // Bind successfully
        }
        close(listenfd);    // Bind failed, try the next
    }

    // Clean up
    freeaddrinfo(listp);
    if (!p) {   // No address worked
        return -1;
    }

    // Make it a listening socket ready to accept connection requests
    if (listen(listenfd, 8) < 0) {    
        close(listenfd);
        return -1;
    }
    return listenfd;
}

int main(int argc, char* argv[]) {
    // Server command
    //char s_argv[MAX_BYTES];

    // Files must be handled before listening to client requests
    // Read in filenames and assign each stock 
    //strcpy(stockList[0].stockName, argv[1]);
    //strcpy(stockList[1].stockName, argv[2]);
    //readFromFile();
    
    // Ctrl + C handler function initialization
    signal(SIGINT, interruptHandler);

    char clientMessage[MAX_BYTES];
    char serverMessage[MAX_BYTES];

    // Socket address and address length initialization
    struct sockaddr_in clientAddress;
    socklen_t  clientLen = sizeof(clientAddress);
    // Open listening descriptor to accept connection request from clients
    int listenfd, connectfd;
    serverfd = open_listenfd(argv[3]);      // passing in the PORT

    printf("Server started\n");
    // Accept connection from client socket
    connectfd = accept(serverfd, (struct sockaddr*)&clientAddress,  &clientLen);
    
    while(1) {
        // set back and clean up
        fflush(stdin);
        fflush(stdout);
        memset(clientMessage, 0, sizeof(clientMessage));
        memset(serverMessage, 0, sizeof(serverMessage));

        // Check if server accepts the connection
        if (connectfd < 0) {
            printf("Failed to accept connection\n");
            return -1;
        }

        // Read client request message 
        if (read(connectfd, clientMessage, sizeof(clientMessage)) < 0) {
            printf("Fail to read client message\n");
            return -1;
        }

        char response[MAX_BYTES] = " is received";
        strcat(clientMessage, response);
        strcpy(serverMessage, clientMessage);

        // Send back the response to client side
        if (send(connectfd, serverMessage, strlen(serverMessage), 0) < 0) {
            printf("Fail to send message to client\n");
            return -1;
        }

        

        // If client ended the program. Do not quit the server
        if (strlen(clientMessage) == 0) {
            printf("Client has ended the connection\n");
            break;
        }
    }

    return 0;
}






