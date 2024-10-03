#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <string>
#include <sys/poll.h>

class Server
{
public:
    explicit Server(int port);

    //setup server
    //false return - smth went wrong
    bool setupSocket();
    bool bindSocketLocally();
    bool findConnection();

    //read message from client
    void getMessage();
    //close sockets and end connection
    void endSession();
    void closeConnection();
    
    bool isMsgFull = true;

    bool connectionStatus;
private:
    //send client "ready" message
    void readyForMessage();

    sockaddr_in servAddrRcv;
    sockaddr_in servAddrSndr;

    int serverSd; // socket descriptor
    int connectSd; // socket 2 connection descriptor

    pollfd fds[1];

    std::string curMsg; // last(current) message
};

#endif // SERVER_H
