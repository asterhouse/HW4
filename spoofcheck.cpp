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
			getpeername(newSD, (sockaddr *) &clientAddr, &addrLen);

			cout << "client addr = " << inet_ntoa(clientAddr.sin_addr);
			cout << " port = " << ntohs(clientAddr.sin_port) << endl;
			
			in_addr_t cSocketAddr = inet_addr(inet_ntoa(clientAddr.sin_addr));
			struct hostent *dnsPtr = gethostbyaddr(&cSocketAddr, sizeof(cSocketAddr), AF_INET);
			
			cout << "official hostname: " << dnsPtr->h_name << endl;

			char *socketAddr = inet_ntoa(clientAddr.sin_addr);

			int nAlias = 0;
			for (char **alias = dnsPtr->h_aliases; *alias != NULL; alias++)
			{
				cout << "alias: " << *alias << endl;
				nAlias++;
			}

			if (nAlias == 0)
			{
				cout << "alias: none" << endl;
			}

			bool trustedClient = false;
			switch (dnsPtr->h_addrtype)
			{
				case AF_INET:

					in_addr *addr;
					for (int i = 0; (addr = (in_addr *)dnsPtr->h_addr_list[i]) != NULL; i++)
					{
						char *reg_ip = inet_ntoa(*addr);
						cout << "ip address: " << reg_ip;
						if (strcmp(socketAddr, reg_ip))
						{
							cout << " ... hit!";
							trustedClient = true;
						}
						cout << endl;
					}
					break;
				default:
					cerr << "unknown address type" << endl;
					break;
			}
			if (trustedClient)
			{
				cout << "an honest client" << endl;
			}

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




