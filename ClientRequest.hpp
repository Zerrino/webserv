/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 05:18:28 by Zerrino           #+#    #+#             */
/*   Updated: 2024/11/08 00:24:36 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTREQUEST_HPP
# define CLIENTREQUEST_HPP
# include "includes/webserv.hpp"


class	ClientRequest
{
	private:
		std::vector<pollfd>	_fds;
		std::vector<int>	_fdSocket;
		int					_fdClient;
		char				_buffer[256];
		std::string			_clientInfo;
		struct sockaddr_in 	_addr;

	public:
		ClientRequest(std::vector<int> fdSocket);
		~ClientRequest();
		ClientRequest(const ClientRequest &cp);
		ClientRequest& operator= (const ClientRequest &cp);

		void	pollRequest();
		void	pollExecute();
		void	acceptRequest(int fd);
		void	sendClient(int fd, std::string path, std::string content_type);
		std::string	getRequest();
		std::string get_clientInfo(int fd);
		std::vector<int>	get_fdSocket();
		int	get_fdClient();
		struct sockaddr_in get_addr();

};

#endif
