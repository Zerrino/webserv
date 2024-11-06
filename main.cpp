#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include "webserv.hpp"

int countChar(std::ifstream file)
{
	std::string line;
	int count = 0;

	while (std::getline(file, line))
		count += line.length();
	return (count);
}

std::vector<std::string> splitInWords(std::ifstream &file)
{
	std::vector<std::string> words;

	std::string word;
	while (!file.eof())
	{

		while (!file.eof()) word.push_back(file.get());
		// if ((file.get() != ' '))
		// 	word += file.get();
	}
	std::cout << word << std::endl;

	return (words);
}

std::vector<Token> tokenize(std::ifstream &file)
{

	std::vector<Token> tokens;
	std::vector<std::string> words;

	words = splitInWords(file);

	// Tokenization logic here

	return (tokens);
}

int main(int ac, char **av)
{
	if (ac > 2)
		return (std::cout << "Invalid number of arguments !" << std::endl, EXIT_FAILURE);
	else
	{
		if (ac == 1)
		{
			int fd_socket;
			int fd_client;

			std::cout << "Start! Using default config file." << std::endl;

			if ((fd_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
				return (strerror(errno), EXIT_FAILURE);

			struct sockaddr_in addr = {
				0, // <-- remove this argument (sin_len, required on macOS) on Linux...
				(AF_INET),
				htons(80), // port 80 converted in hex and in big endian (NBO) in a proper way
				{0},
				{0}};

			if ((bind(fd_socket, (struct sockaddr *)&addr, sizeof(addr))) < 0)
				return (strerror(errno), EXIT_FAILURE);

			if (listen(fd_socket, BACKLOG) < 0)
				return (strerror(errno), EXIT_FAILURE);

			//	if (fcntl(fd_socket, F_SETFL, O_NONBLOCK) < 0) // Switch stream (fd) in non blocking mode for read and write (useful later)
			//		return (strerror(errno), EXIT_FAILURE);

			while (true)
			{
				if ((fd_client = accept(fd_socket, nullptr, nullptr)) < 0)
					return (strerror(errno), EXIT_FAILURE);

				char buffer[BUFFER_SIZE] = {0};
				read(fd_client, buffer, 1024);

				std::cout << "Buffer : " << buffer << std::endl;
				int fd_open = open("index.html", O_RDONLY);

				write(fd_client, "HTTP/1.0 200 OK\nContent-type: text/html\n", 40);
				char buffer2[256];

				ssize_t bytes;
				while ((bytes = read(fd_open, buffer2, BUFFER_SIZE)) > 0)
				{
					write(fd_client, buffer2, bytes);
					std::cout << "Loop" << std::endl;
				}
				close(fd_client);
				close(fd_open);
			}
			close(fd_socket);
		}
		else
		{
			std::vector<Token> tokens;

			std::cout << "Config file provided is : " << std::string(av[1]) << std::endl;
			std::string config_file = std::string(av[1]);
			std::ifstream config_file_stream(config_file);
			if (!config_file_stream.is_open())
				return (EXIT_FAILURE);
			tokenize(config_file_stream);
		}
	}
	return (EXIT_SUCCESS);
}
