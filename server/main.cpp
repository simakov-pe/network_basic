#include "server.h"
#include <iostream>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cerr << "Usage: port" << std::endl;
        exit(0);
    }
    //grab the port number
    int port = atoi(argv[1]);
    Server Srv(port);
    while(1)
    {
        std::string decision = "";
        bool fineStatus = false;
        fineStatus = Srv.setupSocket();
        if (fineStatus = false)
        {
            std::cout << "err in setup Socket" << std::endl;
            std::cout << "Try to resetup?[y, other symbol]" << std::endl;
            std::cin >> decision;
            if (decision.compare("y") == 0 || decision.compare("Y") == 0) continue;
            else break;
        }
        fineStatus = Srv.bindSocketLocally();
        if(fineStatus = false)
        {
            std::cout << "err in bind Socket" << std::endl;
            std::cout << "Try to resetup?[y, other symbol]" << std::endl;
            std::cin >> decision;
            if (decision.compare("y") == 0 || decision.compare("Y") == 0) continue;
            else break;
        }
        fineStatus = Srv.findConnection();
        if (fineStatus = false)
        {
            std::cout << "err in find connection" << std::endl;
            std::cout << "Try to resetup?[y, other symbol]" << std::endl;
            std::cin >> decision;
            if (decision.compare("y") == 0 || decision.compare("Y") == 0) continue;
            else break;
        }

        while(1)
        {
            Srv.getMessage();
            if (Srv.connectionStatus == false)
                break;
            if (Srv.isMsgFull)
            {
                sleep(1);
                continue;
            }
        }
        Srv.endSession();
    }

    return 0;
}
