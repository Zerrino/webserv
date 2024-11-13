/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 05:18:28 by Zerrino           #+#    #+#             */
/*   Updated: 2024/11/13 14:52:31 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientRequest.hpp"

ClientRequest::ClientRequest(std::vector<int> fdSocket)
	:	SendToClient(), _fdSocket(fdSocket)
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
				runcgi(this->_fds[i].fd, "PYTHON");
				this->get_clientInfo(this->_fds[i].fd);
				std::string str = this->_clientInfo;
				std::size_t pos = str.find('\n');
				if (pos != std::string::npos && str.length() >= pos)
					str = str.substr(0, pos);
				std::cout << str << std::endl;
				std::string path = this->getRequest();

				// if (path == "style.css")
				// 	this->sendClient(this->_fds[i].fd, "./data/test/style.css");
				// else if (path == "script.js")
				// 	this->sendClient(this->_fds[i].fd, "./data/test/script.js");
				// else if (path == "favicon.ico")
				// 	this->sendClient(this->_fds[i].fd, "./data/icon/crown.ico");
				// else
				// 	this->sendClient(this->_fds[i].fd, "./data/test/index.html");
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
	if (this->_clientInfo.length() < 6)
		return ("");
	std::string str = this->_clientInfo.substr(5);
	std::size_t pos = str.find(' ');
	if (pos != std::string::npos)
		str = str.substr(0, pos);
	return (str);
}

void	ClientRequest::sendClient(int fd, std::string path)
{
	std::string	str;
	str = this->requestTwo(200, path);
	write(fd, str.c_str(), str.length());
}















