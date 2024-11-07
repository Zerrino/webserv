#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include "Socket.hpp"
#include "ClientRequest.hpp"

int
	main ()
{
	std::cout << "Start!" << std::endl;
	
	Socket sock(AF_INET, SOCK_STREAM, 0);
	sock.runSocket(8080, 10);
	
	int i = 0;
	int max = 10;
	//text/html
	//text/css

	while (i < max)
	{
		try {
			ClientRequest request(sock.get_fdSocket());	
			
			std::cout << request.get_clientInfo() << std::endl;
			std::string path = request.getRequest();
			std::cout << "path : " << path << std::endl;
			
			request.sendClient("index.html", "text/html");

		}
		catch (const std::exception &e) {
			std::cout << "error : " << e.what() << std::endl;
		}
		i++;
	}
	return 0;

}
