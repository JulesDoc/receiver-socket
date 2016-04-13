//============================================================================
// Name        : socketTestServer.cpp
// Author      : Jules
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdio.h>
#include <cstdint>
#include <string.h> //To manipulate data member from sockaddr structures
//#include "boost/asio.hpp"
#include <unistd.h>
#include <boost/cstdint.hpp>
#include "tbb/parallel_for.h"
#include <boost/thread.hpp>
#include <tbb/compat/ppl.h>
//network and socket and socket types preprocessor declarations
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>

#define TRACE

#define PORT 22222
#define BUFSIZE 5
#define NUMMSG 5000

using namespace std;



int main() {

	int datanum = 0;
	struct sockaddr_in hostAddr; //for this host. the receiver
	struct sockaddr_in senderAddr; //for remote host, the sender
	uint32_t senderLen = sizeof(senderAddr); //length of the remote address (could be socklen_t)
	char buffer[BUFSIZE];


	//let's go for the receiver's socket
	int sd = (socket(AF_INET, SOCK_DGRAM, 0));
	if ( sd < 0 ){
		 perror("Error creating socket");
		 return 0;
		}

	//Now, let's set the sockaddr struct to zero
	bzero(&hostAddr, sizeof(hostAddr));

	//Bind the socket, give it a 'name', an address for it - sending this info from host to network.
	//First, fill the socked structure
	hostAddr.sin_family = AF_INET;
	hostAddr.sin_addr.s_addr = htonl(INADDR_ANY); //get any IP address available
    hostAddr.sin_port = htons(PORT); //Get the specified port. See define.
    int binder = bind(sd, (struct sockaddr *)&hostAddr, sizeof(hostAddr));
    if ( binder < 0 ){
    		 perror("Error binding socket");
    		 return 0;
    	}


    //infinite loop for receiving data

    std::cout << "receiver0 waiting" << std::endl;

    	while(true){
    	//let's receive the data


    		uint32_t recibe = recvfrom(sd, buffer, BUFSIZE, 0, (struct sockaddr *)&senderAddr, &senderLen);


    		cout << "Bytes received: " << recibe << endl;
    		cout << "Packet number: " << datanum << endl;

    		datanum++;
    //	if (datanum > NUMMSG) datanum = 0;

   }

   close(sd); //close socket
   return 0;

}










