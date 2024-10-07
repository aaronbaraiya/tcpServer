#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define PORT 4444

//main function
int main() {
    int sockfd, newSocket;    //newsocket is the socket file descriptor
    struct sockaddr_in serverAddr, newAddr;    //used to store the server address
    socklen_t addr_size; //used to store the size of the client address
    char buffer[1024];
    pid_t childpid;

    sockfd = socket(AF_INET, SOCK_STREAM, 0); //create a socket
    if (sockfd < 0) {
        std::cerr << "[-]Error in connection." << std::endl; //error in connection
        exit(1);
    }
    std::cout << "[+]Server Socket is created." << std::endl; //server socket created

    memset(&serverAddr, 0, sizeof(serverAddr)); //clear the server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) { //bind the socket to the server address
        std::cerr << "[-]Error in binding." << std::endl;
        exit(1);
    }
    std::cout << "[+]Bind to port " << PORT << std::endl;

    if (listen(sockfd, 10) == 0) { //listen for incoming connections
        std::cout << "[+]Listening...." << std::endl;
    } else {
        std::cerr << "[-]Error in binding." << std::endl;
    }

    while (true) {
        addr_size = sizeof(newAddr);
        newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size); //accept the incoming connection
        if (newSocket < 0) {
            exit(1);
        }
        std::cout << "Connection accepted from " << inet_ntoa(newAddr.sin_addr) << ":" << ntohs(newAddr.sin_port) << std::endl;

        if ((childpid = fork()) == 0) { //create a child process
            close(sockfd);

            while (true) { //read and send messages to the client
                recv(newSocket, buffer, 1024, 0);
                if (strcmp(buffer, ":exit") == 0) {
                    std::cout << "Disconnected from " << inet_ntoa(newAddr.sin_addr) << ":" << ntohs(newAddr.sin_port) << std::endl;
                    break;
                } else {  //send the message back to the client
                    std::cout << "Client: " << buffer << std::endl;
                    send(newSocket, buffer, strlen(buffer), 0);
                    memset(buffer, 0, sizeof(buffer));
                }
            }
        }
    }

    close(newSocket); //close the socket

    return 0;
}
