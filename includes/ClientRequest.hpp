/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 05:18:28 by Zerrino           #+#    #+#             */
/*   Updated: 2024/11/22 08:38:57 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTREQUEST_HPP
# define CLIENTREQUEST_HPP
# include "webserv.hpp"
# include "SendToClient.hpp"
# include "Cookie.hpp"
# include "Request.hpp"

class	ClientRequest : public SendToClient, public Cookie, public Request
{
	protected:
		std::vector<pollfd>	_fds;
		std::vector<int>	_fdSocket;
		int					_fdClient;
		int					_i;
		// char				_buffer[256];
		std::string			_clientInfo;
		struct sockaddr_in 	_addr;
		std::map<std::string, std::string>	_clMap;

	public:
		ClientRequest(std::vector<int> fdSocket);
		~ClientRequest();

		void	pollRequest();
		void	pollExecute();
		void	acceptRequest(int fd);
		void	sendClient(int fd, int request, std::string path);
		std::string	getRequest();
		std::string get_clientInfo(int fd);
		std::vector<int>	get_fdSocket();
		int	get_fdClient();
		struct sockaddr_in get_addr();

		void	handlingGET(int i);
		void	handlingPUT();
		void	handlingDELETE();
		void	handlingPOST(int i);

};

#endif
