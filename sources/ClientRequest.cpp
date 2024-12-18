/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 05:18:28 by Zerrino           #+#    #+#             */
/*   Updated: 2024/12/18 12:13:58 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ClientRequest.hpp"
#include "../includes/ParseHttp.hpp"
#include "../includes/CGIUtils.hpp"
#include <ctime>


ClientRequest::ClientRequest(std::vector<int> fdSocket)
	:	SendToClient(), _fdSocket(fdSocket), _globReq(0)
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


/*
ClientRequest::ClientRequest(std::vector<int> fdSocket)
	: SendToClient(), _fdSocket(fdSocket), _globReq(0)
{
	// Mise en place de l’epoll
	this->_epoll_fd = epoll_create(1024);
	if (this->_epoll_fd == -1)
	{
		throw std::runtime_error(std::string("epoll_create failed") + strerror(errno));
	}

	// Ajout des sockets d’écoute à l’epoll
	for (size_t i = 0; i < fdSocket.size(); ++i)
	{
		int sfd = fdSocket[i];
		epoll_event event;
		memset(&event, 0, sizeof(event));
		event.data.fd = sfd;
		event.events = EPOLLIN;
		if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, sfd, &event) == -1) {
			throw std::runtime_error(std::string("epoll_ctl(ADD) failed: ") + strerror(errno));
		}
	}
}
*/

void ClientRequest::epollRequest(setOfRuleHTTP rules, HttpBlock fileConfig)
{
	std::vector<epoll_event> events(1000);
	int ret = epoll_wait(this->_epoll_fd, events.data(), (int)events.size(), 1000);
	if (ret == -1)
	{
		if (errno == EINTR)
		{
			return;
		}
		throw std::runtime_error(std::string("epoll_wait failed: ") + strerror(errno));
	}
	for (int i = 0; i < ret; ++i)
	{
		int fd = events[i].data.fd;
		if (std::find(this->_fdSocket.begin(), this->_fdSocket.end(), fd) != this->_fdSocket.end())
		{
			this->acceptRequest(fd);
			int flags = fcntl(this->_fdClient, F_GETFL, 0);
			std::cout << flags << std::endl;
			if (flags == -1) flags = 0;
			fcntl(this->_fdClient, F_SETFL, flags | O_NONBLOCK);
		}
		else
		{
			if (events[i].events & EPOLLIN)
			{
				ParseHttp parser;
				this->get_clientInfo(fd);

				try {
					int	cgi_result;
					std::string locToFollow;
					if (_urlMap.find(_clMap["URI"]) == _urlMap.end())
					{
						_urlMap[_clMap["URI"]].loc = rulingHttp(rules, fileConfig);
						locToFollow = _urlMap[_clMap["URI"]].loc.loc;
						setRulesLoc(locToFollow, rules, fileConfig);
						_urlMap[_clMap["URI"]].rules = rules;
						_urlMap[_clMap["URI"]].str0 = getPath(rules, locToFollow);
						_urlMap[_clMap["URI"]].str1 = _clMap["URI"].substr(_clMap["URI"].size() - 1);
						_urlMap[_clMap["URI"]].str2 = getContentType(_clMap["URI"]);
					}
					locToFollow = _urlMap[_clMap["URI"]].loc.loc;
					rules = _urlMap[_clMap["URI"]].rules;
					this->_path = _urlMap[_clMap["URI"]].str0;
					if (_urlMap[_clMap["URI"]].loc.loc2 == "cgi")
					{
						if (RulesApply(rules, fd))
						{
						}
						else
						{
							cgi_result = CGIchecker(_clMap , _path, rules, fd);
							(void)cgi_result;
						}
					}
					else
					{
						if (RulesApply(rules, fd))
						{
						}
						else
						{
							struct stat buffer;
							if (stat(_path.c_str(), &buffer) != 0)
							{
								if (_clMap.find("GET") != _clMap.end())
									sendClient(fd, 404, "./data/ressources/empty.txt");
								else
								{
									sendClient(fd, 200, "./data/ressources/empty.txt");
								}
							}
							else if (_clMap.find("GET") != _clMap.end())
							{
								handlingGET(fd, rules);
							}
							else if (_clMap.find("POST") != _clMap.end())
							{
								handlingPOST(fd, rules);
							}
							else if (_clMap.find("PUT") != _clMap.end())
							{
								handlingPUT(fd, rules);
							}
							else if (_clMap.find("DELETE") != _clMap.end())
							{
								handlingDELETE(fd, rules);
							}
						}
					}
				}
				catch(const std::exception& e)
				{
					std::cerr << e.what() << '\n';
				}
				if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, fd, 0) == -1)
				{
					std::cerr << "epoll_ctl(DEL) failed: " << strerror(errno) << std::endl;
				}
				close(fd);
			}
		}
	}
}
/*
void ClientRequest::acceptRequest(int fd)
{
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	this->_fdClient = accept(fd, (struct sockaddr *)&addr, &addr_len);
	if (this->_fdClient == -1)
	{
		std::cerr << "accept failed: " << strerror(errno) << std::endl;
		return;
	}
	epoll_event event;
	memset(&event, 0, sizeof(event));
	event.data.fd = this->_fdClient;
	event.events = EPOLLIN;
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, this->_fdClient, &event) == -1)
	{
		std::cerr << "epoll_ctl(ADD, client) failed: " << strerror(errno) << std::endl;
		close(this->_fdClient);
		return;
	}

	//fcntl(this->_fdClient, F_GETFL, 0);
	//fcntl(this->_fdClient, F_SETFL, flags | O_NONBLOCK);
}
*/

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


void	ClientRequest::pollRequest()
{
	int	ret = poll(this->_fds.data(), this->_fds.size(), 3000);
	if (ret <= 0)
		throw std::runtime_error("poll failed");
}

bool ClientRequest::isPrefix(std::string prefix, std::string target)
{
	return target.substr(0, prefix.size()) == prefix;
}

retLoc	ClientRequest::rulingHttp(setOfRuleHTTP &rules, HttpBlock fileConfig)
{
	retLoc	val;
	std::string parentUrl = "";
	size_t len = 0;
	std::string port = _clMap.at("Host").substr(_clMap.at("Host").find(":") + 1);
	ServerBlock serv = *(fileConfig.servers.begin() + rules.listen[port]);
	ParseHttp	parser;
	parser.getDirRules(serv.directives, rules, false);

	for (std::vector<LocationBlock>::iterator i = rules.loc_cgi[rules.listen[port]].begin(); i != rules.loc_cgi[rules.listen[port]].end(); i++)
	{
		if (_clMap["URI"].size() >= i->uri.size() && _clMap["URI"].compare(_clMap["URI"].size() - i->uri.size(), i->uri.size(), i->uri) == 0)
		{
			val.loc = i->uri;
			val.loc2 = "cgi";
			return val; // DONC FINIS PAR EXTENSION CGI
		}
	}
	for (std::vector<LocationBlock>::iterator i = rules.loc_eq[rules.listen[port]].begin(); i != rules.loc_eq[rules.listen[port]].end(); i++)
	{
		if (i->uri == _clMap["URI"])
		{
			val.loc = i->uri;
			val.loc2 = "";
			return val;
		}
	}
	for (std::vector<LocationBlock>::iterator i = rules.loc[rules.listen[port]].begin(); i != rules.loc[rules.listen[port]].end(); i++)
	{
		if (isPrefix(i->uri, _clMap["URI"]) && i->uri.size() > len)
		{
			parentUrl = i->uri;
			len = i->uri.size();
		}
	}
	val.loc = parentUrl;
	val.loc2 = "";
	return val;
}

setOfRuleHTTP	ClientRequest::setRulesLoc(std::string locToFollow, setOfRuleHTTP &rules, HttpBlock fileConfig)
{
	std::string port = _clMap.at("Host").substr(_clMap.at("Host").find(":") + 1);
	ServerBlock serv = *(fileConfig.servers.begin() + rules.listen[port]);
	ParseHttp	parser;

	for (std::vector<LocationBlock>::iterator i = rules.loc_eq[rules.listen[port]].begin(); i != rules.loc_eq[rules.listen[port]].end(); i++)
	{
		if (i->uri == locToFollow)
		{
			parser.getDirRules(i->directives, rules, true);
			return rules;
		}
	}
	for (std::vector<LocationBlock>::iterator i = rules.loc[rules.listen[port]].begin(); i != rules.loc[rules.listen[port]].end(); i++)
	{
		if (i->uri == locToFollow)
		{
			parser.getDirRules(i->directives, rules, true);
			return rules;
		}
	}
	for (std::vector<LocationBlock>::iterator i = rules.loc_cgi[rules.listen[port]].begin(); i != rules.loc_cgi[rules.listen[port]].end(); i++)
	{
		if (i->uri == locToFollow)
		{
			parser.getDirRules(i->directives, rules, true);
			return rules;
		}
	}
	throw std::runtime_error("No location found.");
}

bool	ClientRequest::RulesApply(setOfRuleHTTP rules, int fd)
{
	ParseHttp parser;
	if (rules.root.empty())
		throw std::runtime_error("no root");
	if (!rules.return_code.empty()) // on check le return
	{
		std::cout << "return used!" << std::endl;
		sendClient(fd, parser.fromSTRtoINT(rules.return_code), rules.return_page);
		return true;
	}
	if (!_clMap["Content-Length"].empty() && rules.client_max_body_size != -1)
	{
		if (parser.fromSTRtoLONG(_clMap["Content-Length"]) > rules.client_max_body_size)
		{
			sendClient(fd, 413, "./data/ressources/empty.txt");
			return true;
		}
	}
	if (rules.limit_except.size() > 0)
	{
		bool flag = false;
		for (std::vector<std::string>::iterator i = rules.limit_except.begin(); i != rules.limit_except.end(); i++)
		{
			if (_clMap["METHOD"] == *i)
				flag = true;
		}
		if (flag == false)
		{
			sendClient(fd, 405, "./data/ressources/empty.txt");
			return true;
		}

	}
	return false;
}

std::string ClientRequest::subPath(std::string basePath, std::string fullPath)
{
	if (fullPath.find(basePath) == 0 && basePath.length() < fullPath.length())
		return fullPath.substr(basePath.length());
	return "";
}

std::string ClientRequest::getPath(setOfRuleHTTP rules, std::string locToFollow)
{
	if (rules.root.empty())
		throw std::runtime_error("no root found.");
	std::string path = rules.root;

	if (rules.index.size() == 0)
	{
		if (rules.sub_root.empty())
		{
			path.append(_clMap["URI"]);
		}
		else
		{
			path.append(rules.sub_root);
			path.append(subPath(locToFollow, _clMap["URI"]));
		}
	}
	else
	{
		for (std::vector<std::string>::iterator i = rules.index.begin(); i != rules.index.end(); i++)
		{
			std::string index = *i;
			if (rules.sub_root.empty())
			{
				path.append(_clMap["URI"]);
				if ((_clMap["URI"].substr(_clMap["URI"].size() - 1) == "/") || getContentType(_clMap["URI"]).empty())
				{
					if (getContentType(_clMap["URI"]).empty())
						path.append("/");
					path.append(index);
				}
			}
			else
			{
				path.append(rules.sub_root);
				path.append(subPath(locToFollow, _clMap["URI"]));
				if ((_clMap["URI"].substr(_clMap["URI"].size() - 1) == "/") || getContentType(_clMap["URI"]).empty())
				{
					if (getContentType(_clMap["URI"]).empty())
						path.append("/");
					path.append(index);
				}
			}
			struct stat buffer;
			if (stat(path.c_str(), &buffer) == 0)
				return path;
		}
	}
	return path;
}

void	ClientRequest::pollExecute(setOfRuleHTTP rules, HttpBlock fileConfig)
{
	int	ret = poll(this->_fds.data(), this->_fds.size(), 3000);
	if (ret <= 0)
		return;
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
				ParseHttp parser;
				this->get_clientInfo(this->_fds[i].fd);
				_keepAlive = false;
				if (_clMap["Connection"] != "close")
				{
					_keepAlive = true;
				}
				else if (_clMap["Connection"] == "keep-alive")
				{
					_keepAlive = true;
				}
				//std::cout << "alive " << keepAlive << std::endl;
				try
				{
					int	cgi_result;
					std::string locToFollow;
					if (_urlMap.find(_clMap["URI"]) == _urlMap.end())
					{
						_urlMap[_clMap["URI"]].loc = rulingHttp(rules, fileConfig);
						locToFollow = _urlMap[_clMap["URI"]].loc.loc;
						setRulesLoc(locToFollow, rules, fileConfig);
						_urlMap[_clMap["URI"]].rules = rules;
						_urlMap[_clMap["URI"]].str0 = getPath(rules, locToFollow);
						_urlMap[_clMap["URI"]].str1 = _clMap["URI"].substr(_clMap["URI"].size() - 1);
						_urlMap[_clMap["URI"]].str2 = getContentType(_clMap["URI"]);
					}
					locToFollow = _urlMap[_clMap["URI"]].loc.loc;
					rules = _urlMap[_clMap["URI"]].rules;
					this->_path = _urlMap[_clMap["URI"]].str0;
					if (_urlMap[_clMap["URI"]].loc.loc2 == "cgi")
					{
						if (RulesApply(rules, this->_fds[i].fd))
						{
						}
						else
						{
							cgi_result = CGIchecker(_clMap , _path, rules, _fds[i].fd);
							(void)cgi_result;
						}
					}
					else
					{
						if (RulesApply(rules, this->_fds[i].fd))
						{
						}
						else
						{
							struct stat buffer;
							if (stat(_path.c_str(), &buffer) != 0)
							{
								if (_clMap.find("GET") != _clMap.end())
									sendClient(this->_fds[i].fd, 404, "./data/ressources/empty.txt"); // verifier si la ressource existe bien sur le serveur
								else
								{
									sendClient(this->_fds[i].fd, 200, "./data/ressources/empty.txt");
								}
							}
							else if (_clMap.find("GET") != _clMap.end())
							{
								handlingGET(this->_fds[i].fd, rules);
							}
							else if (_clMap.find("POST") != _clMap.end())
							{
								handlingPOST(this->_fds[i].fd, rules);
							}
							else if (_clMap.find("PUT") != _clMap.end())
							{
								handlingPUT(this->_fds[i].fd, rules);
							}
							else if (_clMap.find("DELETE") != _clMap.end())
							{
								handlingDELETE(this->_fds[i].fd, rules);
							}
						}
					}
				}
				catch(const std::exception& e)

				{
					std::cerr << e.what() << '\n';
				}

				/*
				shutdown(this->_fds[i].fd, SHUT_WR);
				char buf[1024];
				while (read(this->_fds[i].fd, buf, sizeof(buf)) > 0)
				*/
				if (!_keepAlive)
				{
					shutdown(_fds[i].fd, SHUT_WR);
					char buf[1024];
					while (read(this->_fds[i].fd, buf, sizeof(buf)) > 0)
						;
					close(this->_fds[i].fd);
					this->_fds.erase(this->_fds.begin() + i);
					i--;
				}
			}
		}
	}
}

/*
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
*/

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

std::string ClientRequest::get_clientInfo(int fd)
{
	char buffer[4096];
	std::string requestData;
	std::size_t contentLength = 0;
	std::size_t contentRead = 0;
	bool headersParsed = false;

	while (true)
	{
		std::size_t len = read(fd, buffer, sizeof(buffer));
		if (len <= 0)
			break;
		requestData.append(buffer, len);
		if (!headersParsed)
		{
			std::size_t pos = requestData.find("\r\n\r\n");
			if (pos != std::string::npos)
			{
				headersParsed = true;
				std::string headers = requestData.substr(0, pos + 4);
				_clMap = reformat(parseRequest(headers));
				parseContent(_clMap);
				if (_clMap["Transfer-Encoding"] == "chunked")
				{
					clock_t startTime = clock();
					std::string chunkData = requestData.substr(pos + 4);

					_clMap["Content"] = readChunkedBody(fd, chunkData, contentLength);
					std::ostringstream oss;
					oss << contentLength;
					_clMap["Content-Length"] = oss.str();
					clock_t endTime = clock();
					double elapsedTime = static_cast<double>(endTime - startTime) / 100000;
					std::cout << "Temps pris pour parser le body (chunked): " << elapsedTime << " secondes" << std::endl;

				}
				else
				{
					std::stringstream ss(_clMap["Content-Length"]);
					ss >> contentLength;
					std::string contentData = requestData.substr(pos + 4);
					contentRead = contentData.size();
					_clMap["Content"].resize(contentLength);
					_clMap["Content"] = contentData;
					if (contentRead >= contentLength)
						break;
					while (contentRead < contentLength)
					{
						std::size_t len2 = read(fd, buffer, sizeof(buffer));
						if (len2 <= 0)
							break;
						_clMap["Content"].append(buffer, len2);
						contentRead += len2;
					}
				}
				break;
			}
		}
	}

	this->_clientInfo = requestData;
	return requestData;
}


bool ClientRequest::finder(const std::vector<char> &buffer, std::size_t &pos)
{
	for (std::size_t i = 0; i + 1 < buffer.size(); ++i)
	{
		if (buffer[i] == '\r' && buffer[i + 1] == '\n')
		{
			pos = i;
			return true;
		}
	}
	return false;
}


bool ClientRequest::readLine(std::vector<char> &buffer, int fd, std::vector<char> &line)
{
	while (true)
	{
		std::size_t pos;
		if (finder(buffer, pos)) {
			line.assign(buffer.begin(), buffer.begin() + pos);
			buffer.erase(buffer.begin(), buffer.begin() + pos + 2);
			return true;
		}
		char buf[4096];
		ssize_t len = read(fd, buf, sizeof(buf));
		if (len <= 0)
			return false;
		buffer.insert(buffer.end(), buf, buf + len);
	}
}
std::string ClientRequest::readChunkedBody(int fd, std::string &initialBuffer, std::size_t &contentLength)
{
	std::vector<char> tmp_body(0);
	std::string temp = initialBuffer;

	//printMap(_clMap);
	while (true)
	{
		std::string line;
		if (!readLineFromStringOrFd(temp, fd, line))
			break;
		std::size_t chunkSize = 0;
		{
			std::stringstream ss;
			ss << std::hex << line;
			ss >> chunkSize;
		}
		contentLength += chunkSize;
		//std::cout << contentLength << std::endl;
		if (chunkSize == 0)
		{
			std::string emptyLine;
			readLineFromStringOrFd(temp, fd, emptyLine);
			break;
		}
		std::string chunkData;
		chunkData.reserve(chunkSize);
		if (!readBytesFromStringOrFd(temp, fd, chunkSize, chunkData))
			break;
		tmp_body.insert(tmp_body.end(), chunkData.begin(), chunkData.end());
		std::string emptyLine;
		readLineFromStringOrFd(temp, fd, emptyLine);
	}


	std::string body(tmp_body.begin(), tmp_body.end());
	//std::cout << body << std::endl;
	return body;
}

bool ClientRequest::readLineFromStringOrFd(std::string &buffer, int fd, std::string &line)
{
	while (true)
	{
		std::size_t pos = buffer.find("\r\n");
		if (pos != std::string::npos)
		{
			line = buffer.substr(0, pos);
			buffer.erase(0, pos + 2);
			return true;
		}
		char buf[4096];
		std::size_t len = read(fd, buf, sizeof(buf));
		if (len <= 0)
			return false;
		buffer.append(buf, len);
	}
}

bool ClientRequest::readBytesFromStringOrFd(std::string &buffer, int fd, std::size_t length, std::string &data)
{
	//std::cout << length << std::endl;
	while (buffer.size() < length)
	{
		char buf[4096];
		std::size_t len = read(fd, buf, sizeof(buf));
		if (len <= 0)
			return false;
		buffer.append(buf, len);
	}
	data = buffer.substr(0, length);
	buffer.erase(0, length);
	return true;
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

std::string ClientRequest::truncate(std::string str, size_t width)
{
	width--;
	if (str.length() > width)
		return str.substr(0, width - 3) + "...";
	return str;
}

void	ClientRequest::printRequestAnswear(int request, std::string path)
{
	int col1Width = 6;
	int col2Width = 30;
	int col3Width = 80;
	int col4Width = 6;

	std::cout << std::left
	<< std::setw(col1Width) << _clMap.at("METHOD")
	<< std::setw(col2Width) << truncate(_clMap.at("URI"), col2Width)
	<< "| " << request << " " << std::setw(col3Width) << truncate(path, col3Width)
	<< "| port " << std::setw(col4Width) <<truncate(_clMap.at("Host").substr(_clMap.at("Host").find(":") + 1), col4Width)
	<< "| cookie " << _clMap["Cookie_ID"] << " | " << _globReq
	<< std::endl;
	_globReq++;
}

void	ClientRequest::sendClient(int fd, int request, std::string path)
{
	std::string	str = "";
	if ((request >= 100) && (request < 200))
		str = this->requestOne(request, _keepAlive);
	else if ((request >= 200) && (request < 300))
		str = this->requestTwo(request, path, _keepAlive);
	else if ((request >= 300) && (request < 400))
		str = this->requestThree(request, path, _keepAlive);
	else if ((request >= 400) && (request < 500))
		str = this->requestFour(request, path, _keepAlive);
	else if ((request >= 500) && (request < 600))
		str = this->requestFive(request, path, _keepAlive);
	printRequestAnswear(request, path);

	ssize_t total_sent = 0;
	ssize_t to_send = str.length();
	const char *buf = str.c_str();

	while (0 < to_send)
	{
		ssize_t sent = send(fd, buf + total_sent, to_send, MSG_NOSIGNAL);
		if (sent == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				std::cout << "Error" << std::endl;
				break;
			}
			else
			{
				std::cout << "Error2" << std::endl;
				close(fd);
				return;
			}
		}
		else
		{
			total_sent += sent;
			to_send -= sent;
		}
	}
	//send(fd, str.c_str(), str.length(), MSG_NOSIGNAL);
	//write(fd, str.c_str(), str.length());
}




//		void	handlingGET();
//		void	handlingPUT();
//		void	handlingDELETE();
//		void	handlingPOST();

void	ClientRequest::handlingGET(int fd, setOfRuleHTTP rules)
{

	if (_urlMap[_clMap["URI"]].str1 == "/" || _urlMap[_clMap["URI"]].str2.empty())
	{
		if (rules.index.size() == 0)
		{
			this->sendClient(fd, 302, rules.error_page["404"]);
		}
		else //if (_clMap.find("Cookie") != _clMap.end())
		{
			this->sendClient(fd, 200, this->_path);
		}
		/*
		else
		{
			std::string req = "HTTP/1.1 200 OK\r\n";
			//req.append(getDate());
			//req.append("Set-Cookie: session_id=");
			//req.append(createCookieId());
			//req.append("; HttpOnly\r\n");
			std::string file = getFile(PATH_ABS);
			req.append(getContentType(PATH_ABS));
			req.append(this->_length);
			req.append("\r\n\r\n");
			req.append(file);
			write(this->_fds[i].fd, req.c_str(), req.length());
		}
		*/
	}
	else // SI aucune informations en plus
	{
		this->sendClient(fd, 200, this->_path);
	}
}
void	ClientRequest::handlingPUT(int fd, setOfRuleHTTP rules)
{
	(void)rules;
	std::string path = this->_path;
	int	number;
	std::ofstream file(path.c_str());
	if (!file.is_open())
	{
		sendClient(fd, 404, "./data/ressources/empty.txt");
		return ;
	}
	std::istringstream iss(_clMap["Content-Length"]);
	iss >> number;
	file.write(_clMap["Content"].c_str(), number);
	file.close();
	sendClient(fd, 204, "");
}
void	ClientRequest::handlingDELETE(int fd, setOfRuleHTTP rules)
{
	std::string path = this->_path;
	if (std::remove(path.c_str()) == 0)
	{
		sendClient(fd, 204, "");
	}
	else
	{
		sendClient(fd, 302, rules.error_page["404"]);
	}
}
void	ClientRequest::handlingPOST(int fd, setOfRuleHTTP rules)
{
	(void)rules;
	parseContent(_clMap);
	if (_clMap.find("action") != _clMap.end())
	{
		if (_clMap["action"].find("cookieUpdate") != std::string::npos)
		{
			std::string path_cookies = "cookies/";
			int resultCook = isCookies(_clMap["email"], _clMap["password"], "database/profiles.txt");
			if (resultCook == 1)
			{
				cookiedUpdate("login", "true", path_cookies.append(_clMap["Cookie_ID"]));
				cookiedUpdate("session", _clMap["email"].substr(0, _clMap["email"].find('@')), path_cookies);
				sendClient(fd, 204, "");
			}
			else
			{
				cookiedUpdate("login", "false", path_cookies.append(_clMap["Cookie_ID"]));
				if (resultCook == -1)
					sendClient(fd, 404, "./data/ressources/responses/errors/errorkey.txt");
				else
					sendClient(fd, 404, "./data/ressources/responses/errors/errorval.txt");
			}
		}
		else if (_clMap["action"].find("modify") != std::string::npos)
		{
			if (isCookies(_clMap["email"], "a", "database/profiles.txt") == 0)
			{
				cookiedUpdate(_clMap["email"], _clMap["password"], "database/profiles.txt");
				sendClient(fd, 204, "");
			}
			else
				sendClient(fd, 404, "./data/ressources/empty.txt");
		}
		else if (_clMap["action"].find("create") != std::string::npos)
		{
			if (isCookies(_clMap["email"], "", "database/profiles.txt") == -1)
			{
				cookiedUpdate(_clMap["email"], _clMap["password"], "database/profiles.txt");
				sendClient(fd, 204, "");
			}
			else
				sendClient(fd, 404, "./data/ressources/empty.txt");
		}
	}
}

