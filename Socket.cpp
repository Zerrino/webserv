/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 19:35:13 by Zerrino           #+#    #+#             */
/*   Updated: 2024/11/19 13:55:00 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Socket::Socket(int domain, int type, int protocol)
	: _domain(domain), _type(type), _protocol(protocol)
{
	int opt = 1;
	this->_fdSocket = socket(this->_domain, this->_type, this->_protocol);
	if (this->_fdSocket == -1)
		throw std::runtime_error("Socket failed");

	setsockopt(this->_fdSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // a delete
}

Socket::~Socket()
{
	close(this->_fdSocket);
}

Socket::Socket(const Socket& cp)
{
	*this = cp;
}

Socket& Socket::operator= (const Socket& cp)
{
	if (this != &cp)
		*this = cp;
	return (*this);
}

int Socket::get_fdSocket()
{
	return (this->_fdSocket);
}

int Socket::get_backLog()
{
	return (this->_backLog);
}

int Socket::get_domain()
{
	return (this->_domain);
}

int Socket::get_type()
{
	return (this->_type);
}

int Socket::get_protocol()
{
	return (this->_protocol);
}

int Socket::get_host()
{
	return (this->_host);
}

struct sockaddr_in Socket::get_addr()
{
	return (this->_addr);
}

uint16_t Socket::get_port()
{
	return (this->_port);
}

void Socket::bindSocket(sa_family_t sin_family, in_port_t sin_port, struct in_addr sin_addr)
{
	struct sockaddr_in addr = {0};
	addr.sin_family = sin_family;
	addr.sin_port = sin_port;
	addr.sin_addr = sin_addr;
	memset(addr.sin_zero, 0, sizeof(addr.sin_zero)); // Initialiser sin_zero explicitement à 0

	this->_addr = addr;
	int ret = bind(this->_fdSocket, (struct sockaddr *)&addr, sizeof(addr));
	if (ret == -1)
		throw std::runtime_error("Echec de bind ");
}

void Socket::listenSocket(int backlog)
{
	this->_backLog = backlog;
	listen(this->_fdSocket, this->_backLog);
}

void	Socket::runSocket(uint16_t port, int backlog)
{
	this->_port = port;
	try {
		struct in_addr addr;
		addr.s_addr = INADDR_ANY;
		this->bindSocket(AF_INET, htons(port), in_addr(addr));
		this->listenSocket(backlog);
	}
	catch (const std::exception &e)
	{
		std::cout << "error occured : " << e.what() << std::cout;
	}
}
