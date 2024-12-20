/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 05:18:28 by Zerrino           #+#    #+#             */
/*   Updated: 2024/12/20 11:45:12 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTREQUEST_HPP
# define CLIENTREQUEST_HPP
# include "webserv.hpp"
# include "SendToClient.hpp"
# include "Cookie.hpp"
# include "Request.hpp"
# include "ConfigParser.hpp"
# include "CGIRequest.hpp"
# include <sys/epoll.h>

struct retLoc
{
	std::string loc;
	std::string loc2;
};

struct setOfRuleHTTP
{
	int											_epoll_fd;
	long long									client_max_body_size;
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

struct retStr
{
	std::string str0;
	std::string str1;
	std::string str2;
	setOfRuleHTTP	rules;
	retLoc			loc;
};

class	ClientRequest : public SendToClient, public Request
{
	protected:
		int					_epoll_fd;
		std::vector<pollfd>	_fds;
		std::vector<int>	_fdSocket;
		std::string			_path;
		int					_fdClient;
		int					_i;
		int					_globReq;
		bool				_keepAlive;
		int					_err;
		// char				_buffer[256];
		std::string			_clientInfo;
		struct sockaddr_in 	_addr;
		std::map<std::string, std::string>	_clMap;
		std::map<std::string, std::string>	_errorPage;
		std::map<std::string, retStr>	_urlMap;
		//setOfRuleHTTP		_globRules;
	public:
		ClientRequest(std::vector<int> fdSocket);
		~ClientRequest();

		void			pollRequest();
		void			pollExecute(setOfRuleHTTP rules, HttpBlock fileConfig);
		void			acceptRequest(int fd);
		void			sendClient(int fd, int request, std::string path);
		void			sendClient(int fd, int request, std::string path, bool cookie);
		std::string		getRequest();
		std::string 	get_clientInfo(int fd);
		std::vector<int>get_fdSocket();
		int				get_fdClient();
		struct			sockaddr_in get_addr();
		bool 			isPrefix(std::string prefix, std::string target);
		std::string		truncate(std::string str, size_t width);
		void			printRequestAnswear(int request, std::string path);
		void			handlingGET(int i, setOfRuleHTTP rules);
		void			handlingPUT(int i, setOfRuleHTTP rules);
		void			handlingDELETE(int i, setOfRuleHTTP rules);
		void			handlingPOST(int i, setOfRuleHTTP rules);
		retLoc			rulingHttp(setOfRuleHTTP &rules, HttpBlock fileConfig, std::string uri);
		setOfRuleHTTP	setRulesLoc(std::string locToFollow, setOfRuleHTTP rules, HttpBlock fileConfig);
		bool			RulesApply(setOfRuleHTTP rules, int i);
		std::string		getPath(setOfRuleHTTP rules, std::string locToFollow);
		std::string 	subPath(std::string basePath, std::string fullPath);
		bool			readBytes(std::vector<char> &buffer, int fd, std::size_t length, std::vector<char> &data);
		bool			readLine(std::vector<char> &buffer, int fd, std::vector<char> &line);
		bool			readLineFromStringOrFd(std::string &buffer, int fd, std::string &line);
		bool			readBytesFromStringOrFd(std::string &buffer, int fd, std::size_t length, std::string &data);
		std::string		readChunkedBody(int fd, std::string &initialBuffer, std::size_t &contentLength);
		bool			finder(const std::vector<char> &buffer, std::size_t &pos);
		void			listDirconst(std::string &path, int fd);
};

#endif
