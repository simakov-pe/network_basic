#ifndef CLIENT_H
#define CLIENT_H

#include <netinet/in.h>
#include <string>
#include <sys/poll.h>

class Client
{
public:
    explicit Client(char *serverIp, int port);

    //find server, false -smth went wrong
    bool findConnection();

    //send message to server
    bool sendMessage();
    //
    void endSession();
    void closeConnection();

    bool connectionStatus;
private:

    sockaddr_in sendSockAddr;

    int clientSd; // socket descriptor

    pollfd fds[1];

    std::string curMsg; // last(current) message
    int messageNum;
};

#endif // CLIENT_H
