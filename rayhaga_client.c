/*

Jovan Rayhaga

1. Take the server's port number and IP address.
2. Connect to the server using the specified port number and IP address.
3. Ask the user for an input string (maximum size of 256 bytes).
4. Send the length of the string to the server in network byte order.
5. Send the actual string to the server.
6. Receive from the server the number of bytes (in network byte order).
7. Convert the received length back to host order.
8. Receive the actual data from the server based on the length received.
9. Print out the string that the server sends back.

*/

#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define MAX_STRING_SIZE 256 // maximum size of the string the user can input as 256 characters

void userInput(char *input, size_t size) // fuction to get input from user
{
    printf("Enter a string (up to 255 chars): ");
    if (fgets(input, size, stdin) == NULL)
    {
        printf("Invalid input\n"); 
        exit(1);
    }
    input[strcspn(input, "\n")] = 0; // remove the Enter from the user input
}

void sendD(int sd, const char *data) // function for sends the user's input to the server
{
    int length = strlen(data); // get the length of the string
    int convertedLength = htonl(length); // ccnvert the length to network byte order

    if (send(sd, &convertedLength, sizeof(convertedLength), 0) < 0) // send the length of the string to the server first
    {
        perror("Error sending length"); // print error if sending the length to the server fail
        close(sd);
        exit(1);
    }

    if (send(sd, data, length, 0) < 0) // send the actual string data
    {
        perror("Sending data failed"); // print error if sending the actual data to the server fail
        close(sd);
        exit(1);
    }
}

void receiveD(int sd) // function to recieve data from the server
{
    int convertedLength;

    if (recv(sd, &convertedLength, sizeof(convertedLength), 0) < 0) // get length of the string from server

    {
        perror("Error getting length from server"); // print error if receiving length from server fail
        close(sd);
        exit(1);
    }

    int length = ntohl(convertedLength); // convert network byte order to host

    if (length >= MAX_STRING_SIZE) // check if the length is bigger than 256
    {
        printf("More than 256 characters\n");
        close(sd);
        exit(1);
    }

    char receivedData[MAX_STRING_SIZE]; // allocate buffer for receiving data
    memset(receivedData, 0, sizeof(receivedData)); // initialize the buffer with zeros
    
    if (recv(sd, receivedData, length, 0) < 0) // receive the actual data from the server
    {
        perror("Error recieving data"); // print error if receiving the actual data from the server fails
        close(sd);
        exit(1);
    }

    // print the received length and the string
    printf("I am expecting you to send %d bytes\n", length);
    printf("Received the following string '%s'\n", receivedData);
}

int main(int argc, char *argv[]) 
{
    int sd; // socket descriptor
    struct sockaddr_in serverAddr; // store the server's address information

    if (argc < 3) 
    {
        printf("Usage is: <ipAddress> <portNumber>\n");
        exit(1);
    }
    
    char *ipAddress = argv[1]; // store the IP address
    int portNumber = atoi(argv[2]); // convert the port number from a string to an integer


    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) // create socket
    {
        perror("Create socket fail"); // print error if creating the socket fails.
        exit(1);
    }

    // setup server address information
    serverAddr.sin_family = AF_INET;  // use IPv4
    serverAddr.sin_port = htons(portNumber); // convert the port number to network byte order and store it

    if (inet_pton(AF_INET, ipAddress, &serverAddr.sin_addr) <= 0) // convert string IP address to binary
    {
        perror("Invalid IP address"); // print error if converting the IP address fails
        close(sd);
        exit(1);
    }

    if (connect(sd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Error connecting to server"); // Print error if connecting to the server fails.
        close(sd);
        exit(1);
    }

    char inputString[MAX_STRING_SIZE]; // create a buffer to hold the user's input string
    
    userInput(inputString, sizeof(inputString)); // get the user input string
    sendD(sd, inputString);  // send the user input to the server
    receiveD(sd);  // receive the response from the server

    close(sd);
    return 0;
}