#include "client.h"
#include <iostream>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        std::cerr << "Usage: ip_address port" << std::endl; exit(0);
    } //grab the IP address and port number
    char *serverIp = argv[1]; int port = atoi(argv[2]);

    Client Clt(serverIp, port);

    while(1)
    {
        if(!Clt.findConnection())
        {
            sleep(3);
            std::cout << "trying to recconect ..." << std::endl;
            continue;
        }
        while(Clt.sendMessage()){}
        Clt.endSession();
    }
    return 0;
}
