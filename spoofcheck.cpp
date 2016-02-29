#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/uio.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>


using namespace std;

const int BUFFSIZE = 1500;
const int NUM_CONNECTIONS = 10;


int main(int argc, char *argv[])
{

	if (argc != 2)
	{
		cerr << "Incorrect input" << endl;
		return -1;
	}
	
	/* 
     * Build address
     */
	int port = atoi(argv[1]);
    sockaddr_in acceptSocketAddress;
    bzero((char *)&acceptSocketAddress, sizeof(acceptSocketAddress));
    acceptSocketAddress.sin_family = AF_INET;
    acceptSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    acceptSocketAddress.sin_port = htons(port);

    /*
     *  Open socket and bind
     */
    int serverSD = socket(AF_INET, SOCK_STREAM, 0);
    const int on = 1;
    setsockopt(serverSD, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(int));
   
    int rc = bind(serverSD, (sockaddr *)&acceptSocketAddress, sizeof(acceptSocketAddress));
    if (rc < 0)
    {
        cerr << "Bind Failed" << endl;
    }

    /*
     *  listen and accept
     */
    listen(serverSD, NUM_CONNECTIONS);       //setting number of pending connections

	while (true)
	{
		sockaddr_in newSockAddr;
    	socklen_t newSockAddrSize = sizeof(newSockAddr);
    	int newSD = accept(serverSD, (sockaddr *) &newSockAddr, &newSockAddrSize);
		if (fork() == 0)
        {
			sockaddr_in clientAddr;
			socklen_t addrLen = sizeof(clientAddr);
			int peername = getpeername(newSD, (sockaddr *) &clientAddr, &addrLen);            
			cout << "Peer address: " << clientAddr.sin_addr.s_addr << endl;
			cout << "Peer port: " << clientAddr.sin_port << endl;
			cout << "Peer family: " << clientAddr.sin_family << endl;
            close(newSD);
            exit(0);
        }
        else
        {
            close(newSD);
        }
	}


	return 0;
}




