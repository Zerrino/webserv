/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Zerrino <Zerrino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 05:18:28 by Zerrino           #+#    #+#             */
/*   Updated: 2024/11/07 05:18:28 by Zerrino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientRequest.hpp"

ClientRequest::ClientRequest(int fdSocket)
	: _fdSocket(fdSocket)
{
	socklen_t addr_len = sizeof(this->_addr);
	this->_fdClient = accept(this->_fdSocket, (struct sockaddr *)&this->_addr, &addr_len);
	if (this->_fdClient == -1)
		throw std::runtime_error("accept failed");
}

ClientRequest::~ClientRequest()
{
	close(this->_fdClient);
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

int	ClientRequest::get_fdSocket()
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

std::string	ClientRequest::get_clientInfo()
{
	char buffer[1024] = {0};
	read(this->_fdClient, buffer, 1024);
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

void	ClientRequest::sendClient(std::string path, std::string content_type)
{
	int	fdOpen = open(path.c_str(), O_RDONLY);
	ssize_t	bytes;
	char	buffer[256] = {0};
	if (fdOpen == -1)
		throw std::runtime_error("error open, probably path of file");
	write(this->_fdClient, "HTTP/1.0 200 OK\nContent-type: ", 30);
	write(this->_fdClient, content_type.c_str(), content_type.length());
	write(this->_fdClient, "\n", 1);

	while ((bytes = read(fdOpen, buffer, 256)) > 0)
		write(this->_fdClient, buffer, bytes);
	close(fdOpen);
}















