#include "server.h"
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
#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>


Server::Server(int port)
{
    //setup socket
    bzero((char*)&servAddrRcv, sizeof(servAddrRcv));
    bzero((char*)&servAddrSndr, sizeof(servAddrSndr));

    servAddrRcv.sin_family = AF_INET;
    servAddrRcv.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddrRcv.sin_port = htons(port);
}

bool Server::setupSocket()
{
    serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSd < 0)
    {
        std::cerr << "Error establishing the server socket" << std::endl;
        //todo надо ли?
        close(serverSd);
        return false;
    }
    return true;
}

bool Server::bindSocketLocally()
{
    int bindStatus = bind(serverSd, (struct sockaddr*) &servAddrRcv,
        sizeof(servAddrRcv));
    if(bindStatus < 0)
    {
        std::cerr << "Error binding socket to local address" << std::endl;
        close(serverSd);
        return false;
    }
    return true;
}

bool Server::findConnection()
{
    std::cout << "Waiting for a client to connect..." << std::endl;
    listen(serverSd, 5);
    socklen_t servAddrSndrSize = sizeof(servAddrSndr);
    connectSd = accept(serverSd, (sockaddr *)&servAddrSndr, &servAddrSndrSize);
    if (connectSd < 0)
    {
        close(serverSd);
        close(connectSd);
        std::cerr << "Error accepting request from client!" << std::endl;
        return false;
    }
    std::cout << "Connected with client!" << std::endl;
    connectionStatus = true;
    return true;
}

void Server::getMessage()
{
    signal(SIGPIPE, SIG_IGN);
    fds[0].fd = connectSd;
    fds[0].events = POLLIN;

    nfds_t numNfds = 1;

    readyForMessage();
    int ret = poll(fds, numNfds, 100000);
    if ( ret == -1 )
    {
        std::cout << "error getting msg from client" << std::endl;
        closeConnection();
        return;
    }
    else if ( ret == 0 )
    {
        std::cout << "timeout waiting msg from client" << std::endl;
        closeConnection();
        return;
    }
    else
    {
        // обнаружили событие, обнулим revents чтобы можно было переиспользовать структуру
        if ( fds[0].revents & POLLIN )
        {
            fds[0].revents = 0;
            int bufferSize = 100;
            if(isMsgFull)
            {
                isMsgFull = false;
                curMsg.clear();
            }
                //set buffer for 100 chars (if ends with null symbols - message is full
                //                          else - it is part of big message)
                char lastMsg[bufferSize];
                memset(lastMsg, 0, sizeof(lastMsg));
                recv(connectSd, (char*)&lastMsg, sizeof(lastMsg), 0);
                if(!strcmp(lastMsg, "exit"))
                {
                    closeConnection();
                    std::cout << "Client has quit the session normally" << std::endl;
                    return;
                }
                if(!strcmp(lastMsg, "@@@"))
                {
                    getMessage();
                }
                curMsg.append(lastMsg, strlen(lastMsg));
                //if \0 found - message is ended
                if (lastMsg[0] == '\0')
                {
                    isMsgFull = true;
                    std::cout << "zeros readed" << std::endl;
                    closeConnection();
                }
                else if (lastMsg[99] == '\0')
                {
                    isMsgFull = true;
                    std::cout << "Client: " << lastMsg << std::endl;
                }
                else
                    isMsgFull = false;
        }
    }
}

void Server::readyForMessage()
{
   
    char msgToSend[] = "@@@";
    if(send(connectSd, (char*)&msgToSend, strlen(msgToSend), 0) == -1)
        std::cout << "error in send" << std::endl;
    if (errno == EPIPE)
        closeConnection();
}

void Server::endSession()
{
    if(!connectionStatus) return;
    char msgToSend[] = "exit";
    if(send(connectSd, (char*)&msgToSend, strlen(msgToSend), 0) == -1)
        std::cout << "error in send" << std::endl;

    std::cout << "session ended" << std::endl;
    closeConnection();
}

void Server::closeConnection()
{
    if(!connectionStatus) return;
    if (errno == EPIPE)
        std::cout << "connection fault by client" << std::endl;
    std::cout << "sockets closed" << std::endl;
    close(connectSd);
    close(serverSd);
    connectionStatus = false;
}

