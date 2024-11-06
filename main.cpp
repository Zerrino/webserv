#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include "Socket.hpp"


int
	main ()
{
	std::cout << "Start!" << std::endl;
	
	Socket sock(AF_INET, SOCK_STREAM, 0);
	sock.runSocket(8080, 10);
	
	int i = 0;
	int max = 10;
	while (i < max)
	{
		int fd_client = accept(sock.get_fdSocket(), 0, 0);

		char buffer[1024] = {0};
		read(fd_client, buffer, 1024);

		std::cout << "Buffer : " << buffer << std::endl;
		int fd_open = open("index.html", O_RDONLY);
		

		write(fd_client, "HTTP/1.0 200 OK\nContent-type: text/html\n", 40);

		char buffer2[256];
		ssize_t bytes;
		while ((bytes = read(fd_open, buffer2, 256)) > 0)
		{
			write(fd_client, buffer2, bytes);
			std::cout << "Loop" << std::endl;
		}
		close(fd_client);
		close(fd_open);
		i++;
	}
	return 0;

}
