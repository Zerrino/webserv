#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include "../includes/Socket.hpp"
#include "../includes/ClientRequest.hpp"
#include "../includes/SendToClient.hpp"
#include "../includes/Cookie.hpp"
#include "../includes/ConfigParser.hpp"
#include "../includes/Request.hpp"

int main(int ac, char **av)
{
	if (ac > 2)
		return (std::cout << "Invalid number of arguments !" << std::endl, EXIT_FAILURE);
	else
	{
		if (ac == 1)
		{
			srand((unsigned)time(NULL) * getpid());
			std::cout << "Start!" << std::endl;
			/// std::cout << cook.createCookieId() << std::endl;
			// SendToClient test;
			// std::cout << test.getFile("./data/index.html") << std::endl;

			Socket sock(AF_INET, SOCK_STREAM, 0);
			sock.runSocket(57000, 10);

			Socket sock2(AF_INET, SOCK_STREAM, 0);
			sock2.runSocket(50000, 10);

			std::vector<int> ports;
			ports.push_back(sock.get_fdSocket());
			ports.push_back(sock2.get_fdSocket());

			ClientRequest request(ports);
			while (true)
			{
				try
				{
					request.pollRequest();
					request.pollExecute();
				}
				catch (const std::exception &e)
				{
					std::cout << "error : " << e.what() << std::endl;
				}
			}
		}
		else
		{
			ConfigParser config(av[1]);
			ConfigParser::ConfigError status;

			if ((status = config.checkPathValidity()))
				return (std::cerr << config.fetchErrorMsg(status) << std::endl, EXIT_FAILURE);
			if ((status = config.open()))
				return (std::cerr << config.fetchErrorMsg(status) << std::endl, EXIT_FAILURE);
			config.parse();
		}
	}
	return EXIT_SUCCESS;
}
