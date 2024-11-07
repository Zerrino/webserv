/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 05:18:28 by Zerrino           #+#    #+#             */
/*   Updated: 2024/11/08 00:31:20 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientRequest.hpp"

ClientRequest::ClientRequest(std::vector<int> fdSocket)
	:	_fdSocket(fdSocket)
{
	for (std::vector<int>::iterator it = fdSocket.begin(); it != fdSocket.end(); ++it)
	{
		pollfd pfd;
		pfd.fd = *it;
		pfd.events = POLLIN;
		pfd.revents = 0;
		this->_fds.push_back(pfd);
	}
}

void	ClientRequest::pollRequest()
{
	int	ret = poll(this->_fds.data(), this->_fds.size(), 3000);
	if (ret == -1)
		throw std::runtime_error("poll failed");
}

void	ClientRequest::pollExecute()
{
	for (size_t i = 0; i < this->_fds.size(); ++i)
	{
		if (this->_fds[i].revents & POLLIN)
		{
			if (std::find(this->_fdSocket.begin(), this->_fdSocket.end(), this->_fds[i].fd) != this->_fdSocket.end())
			{
				this->acceptRequest(this->_fds[i].fd);
			}
			else
			{
				this->get_clientInfo(this->_fds[i].fd);
				std::string str = this->_clientInfo;
				std::size_t pos = str.find('\n');
				if (pos != std::string::npos)
					str = str.substr(0, pos);
				std::cout << str << std::endl;

				std::string path = this->getRequest();
				if (path == "style.css")
					this->sendClient(this->_fds[i].fd, "./data/test/style.css", "text/css");
				else if (path == "script.js")
					this->sendClient(this->_fds[i].fd, "./data/test/script.js", "text/javascript");
				else if (path == "favicon.ico")
					this->sendClient(this->_fds[i].fd, "./data/icon/crown.ico", "image/x-icon");
				else
					this->sendClient(this->_fds[i].fd, "./data/test/index.html", "text/html");
				close(this->_fds[i].fd);
				this->_fds.erase(this->_fds.begin() + i);
				--i;
			}
		}
	}
}

void	ClientRequest::acceptRequest(int fd)
{
	socklen_t addr_len = sizeof(this->_addr);
	this->_fdClient = accept(fd, (struct sockaddr *)&this->_addr, &addr_len);
	if (this->_fdClient == -1)
		throw std::runtime_error("accept failed");
	pollfd pfd;
	pfd.fd = this->_fdClient;
	pfd.events = POLLIN;
	pfd.revents = 0;
	this->_fds.push_back(pfd);
}

ClientRequest::~ClientRequest()
{
}

ClientRequest::ClientRequest(const ClientRequest& cp)
{
	*this = cp;
}

ClientRequest& ClientRequest::operator= (const ClientRequest& cp)
{
	if (this != &cp)
		*this = cp;
	return (*this);
}

std::vector<int>	ClientRequest::get_fdSocket()
{
	return (this->_fdSocket);
}


int	ClientRequest::get_fdClient()
{
	return (this->_fdClient);
}


struct sockaddr_in ClientRequest::get_addr()
{
	return (this->_addr);
}

std::string	ClientRequest::get_clientInfo(int fd)
{
	char buffer[1024] = {0};
	read(fd, buffer, 1024);
	std::string str(buffer);
	this->_clientInfo = str;
	return (this->_clientInfo);
}

std::string	ClientRequest::getRequest()
{
	std::string str = this->_clientInfo.substr(5);
	std::size_t pos = str.find(' ');
	if (pos != std::string::npos)
		str = str.substr(0, pos);
	return (str);
}

void	ClientRequest::sendClient(int fd, std::string path, std::string content_type)
{
	int	fdOpen = open(path.c_str(), O_RDONLY);
	ssize_t	bytes;
	char	buffer[256] = {0};
	if (fdOpen == -1)
		throw std::runtime_error("error open, probably path of file");
	write(fd, "HTTP/1.0 200 OK\nContent-type: ", 30);
	write(fd, content_type.c_str(), content_type.length());
	write(fd, "\n\n", 2);

	while ((bytes = read(fdOpen, buffer, 256)) > 0)
		write(fd, buffer, bytes);
	close(fdOpen);
}















