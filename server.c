// Thi Thuy Trang Tran, 74889299
// Vik

// Server side
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 58130

#define MAX_BYTES 256
#define MAX_NUMBER_DATE 504     // used for creating data structures to data from csv files
#define MAX_CHAR_DATE 12        // storing date strings
#define MAX_FILENAME 80         // max string for file name
#define MAX_NUMBER_FILE 2

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
}

// data structure for two kinds of stocks. One for MRNA and another one for PFE
struct stock {
    char stockName[MAX_FILENAME]
    //int size;   // size of stock == number of dates in csv
    struct info stockInfo[MAX_NUMBER_DATE];
}stockList[MAX_NUMBER_FILE];   // global stock list. Only contains two stocks: MRNA and PFE

void readFromFile();    // read from File and put data into data structure for each stock

void interruptHandler(int signalNum);       // quit server when Ctrl-C is hit

// Create a listening descriptor that can be used to accept connection requests from clients
// refer to the lecture codes
int open_listenfd(char *port) {
    struct addrinfo hints, *listp, *p;
    int listenfd;
    int optval = 1;

    // Get a list of potential server addresses
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;                // Accept connect..
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;    // ..on any IP address
    hints.ai_flags != AI_NUMERICSERV;               // ..using port no.
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
        if (bind(listenfd, p->ai_addr, p->addrlen) == 0) {
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
    if (listen(listenfd, LISTENQ) < 0) {    
        close(listenfd);
        return -1;
    }
    return listenfd;
}

int main(int argc, char* argv[]) {
    // Server command
    //char s_argv[MAX_BYTES];
    int serverSocket = -1;
    int clientSocket = -1;

    // Files must be handled before listening to client requests
    // Read in filenames and assign each stock 
    strcpy(stockList[0].stockName, argv[1]);
    strcpy(stockList[1].stockName, argv[2]);
    //readFromFile();

    // Ctrl + C handler function initialization
    signal(SIGINT, interruptHandler);

    char clientMessage[MAX_BYTES];
    char serverMessage[MAX_BYTES];

    // Socket address and address length initialization
    struct sockaddr_in clientAddress;
    int clientLen = sizeof(clientAddress);
    // Open listening descriptor to accept connection request from clients
    int listenfd, connectfd;
    listenfd = open_clientfd(argv[3]);      // passing in the PORT
    printf("Server started\n");
    // 
    connectfd = accept(listenfd, (SA *)&clientAddress,  &clientLen);


}






