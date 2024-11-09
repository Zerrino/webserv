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
#include "SendToClient.hpp"

int
	main ()
{
	std::cout << "Start!" << std::endl;

	Socket sock(AF_INET, SOCK_STREAM, 0);
	sock.runSocket(9000, 10);

	Socket sock2(AF_INET, SOCK_STREAM, 0);
	sock2.runSocket(8080, 10);

	int i = 0;
	int max = 10000000;
	std::vector<int> ports;
	ports.push_back(sock.get_fdSocket());
	ports.push_back(sock2.get_fdSocket());

	ClientRequest	request(ports);
	while (i < max)
	{
		try
		{
			request.pollRequest();
			request.pollExecute();
		}
		catch (const std::exception &e) {
			std::cout << "error : " << e.what() << std::endl;
		}
		i++;
	}

	return 0;
}
