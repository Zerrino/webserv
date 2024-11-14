/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdelvign <gdelvign@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 19:35:13 by Zerrino           #+#    #+#             */
/*   Updated: 2024/11/14 15:11:44 by gdelvign         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

#include "webserv.hpp"

class	Socket
{
	private:
		int _fdSocket;
		int _backLog;
		int _domain;
		int _type;
		int _protocol;
		int _host;
		uint16_t _port;
		struct sockaddr_in _addr;
	public:
		Socket(int domain, int type, int protocol);
		~Socket();
		Socket(const Socket &cp);
		Socket& operator= (const Socket &cp);

		void	bindSocket(sa_family_t sin_family, in_port_t sin_port, struct in_addr sin_addr);
		void	listenSocket(int backlog);

		void	runSocket(uint16_t port, int backlog);

		uint16_t get_port();
		int get_fdSocket();
		int get_backLog();
		int get_domain();
		int get_type();
		int get_protocol();
		int get_host();
		struct sockaddr_in get_addr();
};

#endif
