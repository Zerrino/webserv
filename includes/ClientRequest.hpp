/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 05:18:28 by Zerrino           #+#    #+#             */
/*   Updated: 2024/12/10 13:30:42 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTREQUEST_HPP
# define CLIENTREQUEST_HPP
# include "webserv.hpp"
# include "SendToClient.hpp"
# include "Cookie.hpp"
# include "Request.hpp"
# include "ConfigParser.hpp"

struct setOfRuleHTTP
{
	int											client_max_body_size;
	std::map<std::string, int>					listen; // ne pas remplir
	std::map<std::string, std::string>			error_page;
	std::string									server_name;
	std::string									root;
	std::string									sub_root; // seulement dans les locations
	std::vector<std::string>					limit_except;
	std::string									autoindex;
	std::string									fastcgi_param;
	std::vector<std::string>					index;
	std::string									return_code;
	std::string									return_page;
	std::map<int, std::vector<LocationBlock> >	loc;
	std::map<int, std::vector<LocationBlock> >	loc_eq;
	std::map<int, std::vector<LocationBlock> >	loc_cgi;
	setOfRuleHTTP()
	: client_max_body_size(-1),
		server_name(""),
		root(""),
		sub_root(""),
		autoindex(""),
		fastcgi_param(""),
		return_code(""),
		return_page("")
		{}
};



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
		void	pollExecute(setOfRuleHTTP rules, HttpBlock fileConfig);
		void	acceptRequest(int fd);
		void	sendClient(int fd, int request, std::string path);
		std::string	getRequest();
		std::string get_clientInfo(int fd);
		std::vector<int>	get_fdSocket();
		int	get_fdClient();
		struct sockaddr_in get_addr();

		bool isPrefix(std::string prefix, std::string target);
		std::string truncate(std::string str, size_t width);
		void	printRequestAnswear(int request, std::string path);
		void	handlingGET(int i, setOfRuleHTTP rules);
		void	handlingPUT(int i, setOfRuleHTTP rules);
		void	handlingDELETE(int i, setOfRuleHTTP rules);
		void	handlingPOST(int i, setOfRuleHTTP rules);
		std::string	rulingHttp(setOfRuleHTTP &rules, HttpBlock fileConfig);
		void	setRulesLoc(std::string locToFollow, setOfRuleHTTP &rules, HttpBlock fileConfig);
		bool	RulesApply(setOfRuleHTTP rules, int i);
};

#endif
