/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 05:18:28 by Zerrino           #+#    #+#             */
/*   Updated: 2024/11/08 22:26:41 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTREQUEST_HPP
# define CLIENTREQUEST_HPP
# include "includes/webserv.hpp"
# include "SendToClient.hpp"

class	ClientRequest : public SendToClient
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
