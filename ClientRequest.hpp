/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Zerrino <Zerrino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 05:18:28 by Zerrino           #+#    #+#             */
/*   Updated: 2024/11/07 05:18:28 by Zerrino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTREQUEST_HPP
# define CLIENTREQUEST_HPP
# include "includes/webserv.hpp"


class	ClientRequest
{
	private:
		int			_fdClient;
		int			_fdSocket;
		char		_buffer[256];
		std::string	_clientInfo;
		struct sockaddr_in _addr;
	
	public:
		ClientRequest(int fdSocket);
		~ClientRequest();
		ClientRequest(const ClientRequest &cp);
		ClientRequest& operator= (const ClientRequest &cp);

		void	sendClient(std::string path, std::string content_type);
		std::string	getRequest();
		std::string get_clientInfo();
		int	get_fdSocket();
		int	get_fdClient();
		struct sockaddr_in get_addr();
		
};

#endif
