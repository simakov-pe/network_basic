#include "client.h"
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include <signal.h>


Client::Client(char* serverIp, int port)
{
    struct hostent* host = gethostbyname(serverIp);
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr =
        inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);

    connectionStatus = false;
    messageNum = 0;
}

bool Client::findConnection()
{
    clientSd = socket(AF_INET, SOCK_STREAM, 0);
    int status = connect(clientSd, (sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
    if(status < 0)
    {
        std::cout << "Error connecting to socket!"<< std::endl;
        close(clientSd);
        connectionStatus = false;
        return false;
    }
    connectionStatus = true;
    std::cout << "Connected to the server!" << std::endl;
    return true;
}

bool Client::sendMessage()
{
    signal(SIGPIPE, SIG_IGN);
    char servAnswer[10];
    memset(&servAnswer, 0, sizeof(servAnswer));
    fds[0].fd = clientSd;
    fds[0].events = POLLIN;
    nfds_t numNfds = 1;

    int ret = poll(fds, numNfds, 100000);
    if (ret == -1)
    {
        std::cout << "error getting msg from server" << std::endl;
        closeConnection();
        return false;
    }
    else
    {
        if ( fds[0].revents & POLLIN )
        {
            fds[0].revents = 0;
            recv(clientSd, (char*)&servAnswer, sizeof(servAnswer), 0);

            if(!strcmp(servAnswer, "exit"))
            {
                endSession();
                std::cout << "server ended" << std::endl;
                return false;
            }
            else if(!strcmp(servAnswer, "@@@"))
            {
                char message[100];
                memset(&message, 0, sizeof(message));
                sprintf(message, "this is %d message", messageNum);
                if(send(clientSd, (char*)&message, strlen(message) + 1, 0) == -1)
                {
                    std::cout << "error in send" << std::endl;
                    closeConnection();
                    return false;
                }
                std::string msgStr(message);
                std::cout << msgStr << std::endl;
                messageNum++;
            }
            else
            {
                std::cout << "wrong answer from server" << std::endl;
                endSession();
                return false;
            }
            return true;
        }
    }

}

void Client::endSession()
{
    if(!connectionStatus) return;
    char msgToSend[] = "exit";
    if(send(clientSd, (char*)&msgToSend, strlen(msgToSend), 0) == -1)
    {
        std::cout << "error in send" << std::endl;
    }
    std::cout << "session ended" << std::endl;
    closeConnection();
}

void Client::closeConnection()
{
    if(!connectionStatus) return;
    if(errno == EPIPE)
        std::cout << "connection fault by server" << std::endl;
    std::cout << "sockets closed" << std::endl;
    close(clientSd);
    connectionStatus = false;
}
