// Thi Thuy Trang Tran, 74889299
// Vikasni Kalahasthi 78601545
//Note: Dates are entered as YYYY-MM-DD, but methods here compare against DD/MM/YYYY. 
//Formay of input must be modified before using the methods for PriceOnDate and MaxPossibleProfit
// for now: when client quits, server will also quit -> will remove that later

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
#include <math.h>
#define PORT 58130

#define MAX_BYTES 256
#define MAX_NUMBER_STOCKS 503     // used for creating data structures to data from csv files
#define MAX_CHAR_DATE 12        // storing date strings
#define MAX_FILENAME 80         // max string for file name
#define MAX_NUMBER_FILE 2
#define MAX_LINE 80

// Global socket file descriptors. Only closes when Ctrl + C is hit (will be handled in interruptHandler)
int serverfd = -1;     
int connectfd = -1;

void distributeInput(char* input, int* argc, char** argv) { // distributes input into argc & argv
    /*char* token;        
    const char* delims = " \t\n";
    token = strtok(input, delims);      // first token is the command
    while (token != NULL) {             // getting next arguments in to argv
        argv[(*argc)++] = token;
        token = strtok(NULL, delims);
    }*/
    char* token = strtok(input, " \t\n");
    *argc = 0;

    while (token != NULL) {
        argv[(*argc)++] = token;
        token = strtok(NULL, " \t\n");
    }
    argv[(*argc)] = token;
}

// stock price information. Including date and closing price associated with date
struct info {
    char date[MAX_CHAR_DATE];
    double closingPrice;

};

// data structure for two kinds of stocks. One for MRNA and another one for PFE
struct stock {
    char stockName[MAX_FILENAME];
    int size;   // size of stock == number of dates in csv
    struct info stockInfo[MAX_NUMBER_STOCKS];
}stockList[MAX_NUMBER_FILE];   // global stock list. Only contains two stocks: MRNA and PFE



void readFromFiles(int index){ // read from File and put data into data structure for each stock
    //0 is always PFE and 1 is always MRNA
    FILE* fd;
    if(index == 0){
        printf("Reading PFE stocks.\n");
        fd = fopen("./PFE.csv","r");
    }
    else{
        printf("Reading MRNA stocks.\n");
        fd = fopen("./MRNA.csv","r");
    }
    
    char buffer[1024];
    int row = 0;
    int column = 0;
    int counter = 0;
    char *eptr;
    while(fgets(buffer,1024,fd)){
        column = 0;
        row++;
        if(row == 1){ //skip first row
            continue;
        }
        char* value = strtok(buffer,",");
        while(value){
            if(column == 0){
               
               strcpy(stockList[index].stockInfo[counter].date,value);
            }
            if(column == 4){
              //printf(" Value Inputted: %s\n",value);
              stockList[index].stockInfo[counter].closingPrice = strtod(value,&eptr);
              //printf(" Value Inputted: %f\n",stockList[index].stockInfo[counter].closingPrice);
            }
            value = strtok(NULL,",");
            column++;


        }
        counter++;
        //print line if needed
    }
    fclose(fd);
}

void smarterReadFromFiles(int index) {
    //0 is always PFE and 1 is always MRNA
    FILE* fd;
    int size = 0;

    if(index == 0){
        printf("Reading PFE stocks.\n");
        // can just assign name here too
        fd = fopen("./PFE.csv", "rb");
    }
    else{
        printf("Reading MRNA stocks.\n");
        // assign name here
        fd = fopen("./MRNA.csv", "rb");
    }

    //Ignore the first line
    fscanf(fd, "%*[^\n]\n");
    // Only read first and fifth column into right formatted date and price
    while(fscanf(fd, "%10s,%*[^,],%*[^,],%*[^,],%lf,%*[^\n]\n", stockList[index].stockInfo[size].date, &stockList[index].stockInfo[size].closingPrice) != EOF) {
        // Initialize
        stockList[index].stockInfo[size++].date[MAX_CHAR_DATE] = 0;
        stockList[index].size = size;
    }

    fclose(fd);

}



void printPFEStockList(){ //helper function
    printf("Reading %s stocks\n",stockList[0].stockName);
    for(int i = 0; i < stockList[0].size; i++){
        printf("%s | %f\n",stockList[0].stockInfo[i].date, stockList[0].stockInfo[i].closingPrice);
    }
}
void printMRNAStockList(){ //helper function
    //printf("Reading %s stocks\n",stockList[1].stockName);
    for(int i = 0; i < MAX_NUMBER_STOCKS;i++){
        
        //printf("%s | %f\n",stockList[1].stockInfo[i].date,stockList[1].stockInfo[i].closingPrice);
    }
}



double maxPossibleProfit_Loss(char* type,char* stockName,char* startTime, char* endTime){
    //figure out dimensions by finding out how many days are in between start and end time
    //make the profit array
    double profit[MAX_NUMBER_STOCKS];
    int counter = 0;
    int flag = 0;
    int index;
    if(strcmp(stockName,"PFE")== 0){
        index = 0; //PFE
    }
    else{
        index = 1; //MRNA
    }

    for(int i = 0; i < MAX_NUMBER_STOCKS; i++){
        if(strcmp(stockList[index].stockInfo[i].date,startTime) == 0){
            //printf("Start date found.\n");
            flag = 1;
            profit[counter] = stockList[index].stockInfo[i].closingPrice;
            counter++;
        }
        if(flag == 1 && strcmp(stockList[index].stockInfo[i].date,startTime) != 0){
            profit[counter] = stockList[index].stockInfo[i].closingPrice;
            counter++;
        }
        if(strcmp(stockList[index].stockInfo[i].date,endTime) == 0){
            flag = 0;
        }
    }
    
    for(int i = 0; i < counter; i++){
        //printf("%f,",profit[i]);
    }
    //printf("\n");

    
    double answer = 0;
    int buyDay = 0;
    int sellDay = 0;

    if(strcmp(type,"profit") == 0){ //if the type is profit calc profit
        for( int i = 0; i < counter;i++){
                for(int j = i; j < counter; j++){
                    if(answer <= (profit[j]-profit[i])){
                        answer = profit[j]-profit[i];
                        buyDay = i;
                        sellDay = j;
                    }
                }
        }
    }
    else{ //calc loss
        for( int i = 0; i < counter;i++){
                for(int j = i; j < counter; j++){
                    if(answer > (profit[j]-profit[i])){
                        answer = profit[j]-profit[i];
                        buyDay = i;
                        sellDay = j;
                    }
                }
        }
    }
   

    answer= fabs(answer);
    return answer; 
    
}


/*char* pricesOnDate(char* date){
   //assumes that both lists have the same dates and in the same order
    char result[MAX_LINE] = "PFE: ";
    char*r_result = "test";
    char PFEnum[MAX_LINE];
    char MRNAnum[MAX_LINE];
    for(int i = 0; i < MAX_NUMBER_STOCKS;i++){
        if(strcmp(stockList[0].stockInfo[i].date,date) == 0 && strcmp(stockList[1].stockInfo[i].date,date) == 0){
            printf("number found\n");
            sprintf(PFEnum, "%.2f",stockList[0].stockInfo[i].closingPrice);
            sprintf(MRNAnum,"%.2f",stockList[1].stockInfo[i].closingPrice);
            strcat(result, PFEnum);
            strcat(result," | MRNA: ");
            strcat(result, MRNAnum); 
        }
    }
    char *res = result;
    //printf("%s\n",result);
    //strcpy(r_result,result);
    return res;
}*/

void pricesOnDate(char* date, char* message) {
    //assumes that both lists have the same dates and in the same order
    char result[MAX_LINE] = "PFE: ";
    //char*r_result = "test";
    char PFEnum[MAX_LINE];
    char MRNAnum[MAX_LINE];
    for(int i = 0; i < MAX_NUMBER_STOCKS;i++){
        if(strcmp(stockList[0].stockInfo[i].date,date) == 0 && strcmp(stockList[1].stockInfo[i].date,date) == 0){
            printf("number found\n");
            sprintf(PFEnum, "%.2f",stockList[0].stockInfo[i].closingPrice);
            sprintf(MRNAnum,"%.2f",stockList[1].stockInfo[i].closingPrice);
            strcat(result, PFEnum);
            strcat(result," | MRNA: ");
            strcat(result, MRNAnum); 
        }
    }
    strcpy(message, result);
}

/*char* eval(char **argv, int argc) {         // will return an arbitrary string when argv[0] is not PricesOnDate. need UPDATES !!
    char *result;
    printf("In eval: \n");
    printf("argv[1] : %s", argv[1]);
    if(strcmp(argv[0], "PricesOnDate") == 0){
       strcpy(result, pricesOnDate(argv[1]));
    }
    

    return result;
}*/

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

void reformatDate(char* date) {
    //char retDate[MAX_LINE];
    int yearMonthDay[3];    // array to store year, month and day respectively
    // extract and convert into each category
    char* token;
    int i = 0;
    const char* delims = "-";
    token = strtok(date, delims);       // first token is the year
    while (token != NULL) {             // second is month and third is day
        yearMonthDay[i++] = atoi(token);
        token = strtok(NULL, delims);
    }
    sprintf(date, "%d/%d/%d", yearMonthDay[1], yearMonthDay[2], yearMonthDay[0]);
}

int main(int argc, char* argv[]) {

    // Files must be handled before listening to client requests
    // Read in filenames and assign each stock - first is always PFE and second is always MRNA
    strcpy(stockList[0].stockName, argv[1]);
    strcpy(stockList[1].stockName, argv[2]);
    readFromFiles(0);
    readFromFiles(1);
    //smarterReadFromFiles(0);
    //smarterReadFromFiles(1);
    
    double pro = maxPossibleProfit_Loss("profit","PFE","9/11/2019","10/15/2019");
    double loss = maxPossibleProfit_Loss("loss","MRNA","4/16/2020","8/23/2020");
    //sprintf("Max profit: %.2f\nMax Loss: %.2f\n",pro,loss);
    //need to make an array with the stocks from the start date to the end date, then send it to the max profit or loss


    //printPFEStockList();
    //printMRNAStockList();
    //printf("%s\n",pricesOnDate("7/9/2019"));
    //printf("%s\n",pricesOnDate("1/17/2020"));
    

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

        // Server command
        char input[MAX_LINE];
        char* u_argv[MAX_LINE];
        int u_argc = 0;


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
        
        printf("%s\n", clientMessage);
        // Distribute client request to correct format to arguments
        distributeInput(clientMessage, &u_argc, u_argv);
        for (int i = 0; i < u_argc; i++) {
            printf("u_argv[%d] = %s ", i, u_argv[i]);
        }
        printf("\n");

        if (strcmp(u_argv[0], "PricesOnDate") == 0) {
            char priceMessage[MAX_BYTES];
            printf("date: %s", u_argv[1]);
            reformatDate(u_argv[1]);
            printf("reformatted date: %s: ", u_argv[1]);
            pricesOnDate(u_argv[1], priceMessage);
            strcpy(serverMessage, priceMessage);
        }

        //char response[MAX_BYTES];
        //strcpy(response, eval(u_argv, u_argc));
        //strcpy(serverMessage, response);

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





