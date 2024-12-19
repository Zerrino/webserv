/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 05:18:28 by Zerrino           #+#    #+#             */
/*   Updated: 2024/12/19 20:32:28 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ClientRequest.hpp"
#include "../includes/ParseHttp.hpp"
#include "../includes/CGIUtils.hpp"
#include <ctime>
#include <dirent.h>
#include <unistd.h>

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

retLoc	ClientRequest::rulingHttp(setOfRuleHTTP &rules, HttpBlock fileConfig, std::string uri)
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
		if (uri.size() >= i->uri.size() && uri.compare(uri.size() - i->uri.size(), i->uri.size(), i->uri) == 0)
		{
			val.loc = i->uri;
			val.loc2 = "cgi";
			return val; // DONC FINIS PAR EXTENSION CGI
		}
	}
	for (std::vector<LocationBlock>::iterator i = rules.loc_eq[rules.listen[port]].begin(); i != rules.loc_eq[rules.listen[port]].end(); i++)
	{
		if (i->uri == uri)
		{
			val.loc = i->uri;
			val.loc2 = "";
			return val;
		}
	}
	for (std::vector<LocationBlock>::iterator i = rules.loc[rules.listen[port]].begin(); i != rules.loc[rules.listen[port]].end(); i++)
	{
		if (isPrefix(i->uri, uri) && i->uri.size() > len)
		{
			parentUrl = i->uri;
			len = i->uri.size();
		}
	}
	val.loc = parentUrl;
	val.loc2 = "";
	return val;
}

setOfRuleHTTP	ClientRequest::setRulesLoc(std::string locToFollow, setOfRuleHTTP rules, HttpBlock fileConfig)
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
		//std::cout << "return used!" << std::endl;
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
	std::string path_copy;
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
			path = rules.root;
			std::string index = *i;
			if (rules.sub_root.empty())
			{
				path.append(_clMap["URI"]);
				if ((_clMap["URI"].substr(_clMap["URI"].size() - 1) == "/") || getContentType(_clMap["URI"]).empty())
				{
					if (getContentType(_clMap["URI"]).empty())
						path.append("/");
					path_copy = path;
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
					path_copy = path;
					path.append(index);
				}
			}
			struct stat buffer;
			if (stat(path.c_str(), &buffer) == 0)
				return path;
		}
	}
	//std::cout << path_copy << std::endl;
	if (!path_copy.empty())
		return path_copy;
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
				//printMap(_clMap);
				_keepAlive = false;
				_err = 0;



				if (_clMap["Connection"] == "keep-alive")
				{
					_keepAlive = true;
				}
				else if (_clMap["Connection"] != "close")
				{
					_keepAlive = true;
				}

				try
				{
					int	cgi_result;
					std::string locToFollow;
					_urlMap[_clMap["URI"]].loc = rulingHttp(rules, fileConfig, _clMap["URI"]);
					locToFollow = _urlMap[_clMap["URI"]].loc.loc;
					rules = setRulesLoc(locToFollow, rules, fileConfig);
					_urlMap[_clMap["URI"]].rules = rules;
					_urlMap[_clMap["URI"]].str0 = getPath(rules, locToFollow);
					_urlMap[_clMap["URI"]].str1 = _clMap["URI"].substr(_clMap["URI"].size() - 1);
					_urlMap[_clMap["URI"]].str2 = getContentType(_clMap["URI"]);
					_errorPage.clear();
					for (std::map<std::string, std::string>::iterator it = rules.error_page.begin(); it != rules.error_page.end(); ++it)
					{
						setOfRuleHTTP errorRules;
						std::string errorLoc;
						retLoc errorRetLoc;
						std::string	errorPath;
						errorRetLoc = rulingHttp(rules, fileConfig, it->second);
						errorLoc = errorRetLoc.loc;
						errorRules = setRulesLoc(errorLoc, rules, fileConfig);
						errorPath = getPath(errorRules, errorLoc) + it->second;
						_errorPage[it->first] = errorPath;
					}
					//parser.printSetOfRuleHTTP(rules);
					locToFollow = _urlMap[_clMap["URI"]].loc.loc;
					rules = _urlMap[_clMap["URI"]].rules;
					this->_path = _urlMap[_clMap["URI"]].str0;
					//_globRules = rules;
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
							//std::cout << _path << std::endl;
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
					std::cerr << "error : " << e.what() << '\n';
				}

				if (!_keepAlive || _err == 1)
				{
					//shutdown(_fds[i].fd, SHUT_WR);
					//char buf[1024];
					//while (read(this->_fds[i].fd, buf, sizeof(buf)) > 0)
					//	;
					close(this->_fds[i].fd);
					this->_fds.erase(this->_fds.begin() + i);
					i--;
					_err = 0;
				}
			}
		}
	}
}


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
	std::string	request_str = "";
	std::stringstream ss;
	ss << request;
	request_str = ss.str();
	//std::cout << request_str << std::endl;
	if (_errorPage.find(request_str) != _errorPage.end())
	{
		struct stat buffer;
		if (stat(_errorPage[request_str].c_str(), &buffer) == 0)
			path = _errorPage[request_str];
	}

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
				break;
			}
			else
			{
				_err = 1;
				return;
			}
		}
		else
		{
			total_sent += sent;
			to_send -= sent;
		}
	}
}



void ClientRequest::listDirconst(std::string &path, int fd)
{
	std::size_t count = 0;
	DIR *dir = opendir(path.c_str());
	if (!dir)
	{
		sendClient(fd, 404, "./data/ressources/empty.txt");
		return;
	}
	std::ofstream file("./tmp/json_folder/tmp_autoindex.json", std::ios::out | std::ios::trunc);
	if (!file)
	{
		sendClient(fd, 404, "./data/ressources/empty.txt");
		return;
	}
	file << "{" << std::endl;
	file << "\t\"files\": [" << std::endl;
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		if (count != 0)
			file << "," << std::endl;
		std::string fullPath = path;
		if (fullPath[fullPath.size() - 1] != '/')
			fullPath += "/";
		fullPath += entry->d_name;

		struct stat st;
		if (stat(fullPath.c_str(), &st) == -1) // unable top open fichier/dossier
			continue;
		char type = '?';
		if (S_ISREG(st.st_mode))  type = 'F'; // Fichier
		if (S_ISDIR(st.st_mode))  type = 'D'; // Dossier
		if (S_ISLNK(st.st_mode))  type = 'L'; // Lien symbolique
		if (S_ISCHR(st.st_mode))  type = 'C'; // Peripherique caractere
		if (S_ISBLK(st.st_mode))  type = 'B'; // Peripherique bloc
		if (S_ISFIFO(st.st_mode)) type = 'P'; // FIFO/pipe nomme
		if (S_ISSOCK(st.st_mode)) type = 'S'; // Socket
		file << "\t\t{" << std::endl
			<< "\t\t\t\"" << "name" << "\": \"" << entry->d_name << "\"," << std::endl
			<< "\t\t\t\"" << "type" << "\": \"" << type << "\"," << std::endl
			<< "\t\t\t\"" << "perm" << "\": \"" << (st.st_mode & 07777) << "\"," << std::endl
			<< "\t\t\t\"" << "size" << "\": " << st.st_size << "" << std::endl
			<< "\t\t}";
		count++;
	}
	file << std::endl << "\t]";
	file << std::endl << "}" << std::endl;
	file.close();
	closedir(dir);
	sendClient(fd, 200, "./tmp/json_folder/tmp_autoindex.json");
}

void	ClientRequest::handlingGET(int fd, setOfRuleHTTP rules)
{
	struct stat buffer;
	stat(_path.c_str(), &buffer);
	bool isFolder = S_ISDIR(buffer.st_mode);
	if (rules.autoindex == "on" && isFolder)
	{
		listDirconst(_path, fd);
	}
	else if (rules.autoindex != "on" && isFolder)
	{
		sendClient(fd, 404, "./data/ressources/empty.txt");
	}
	else if (_urlMap[_clMap["URI"]].str1 == "/" || _urlMap[_clMap["URI"]].str2.empty())
	{
		if (rules.index.size() == 0)
		{
			this->sendClient(fd, 404, "./data/ressources/empty.txt");
		}
		else if ((_clMap.find("Cookie") == _clMap.end()) && (_clMap.find("User-Agent") != _clMap.end()))
		{
			std::string req = "HTTP/1.1 200 OK\r\n";
			if (_keepAlive)
				this->_request.append("Connection: keep-alive\r\n");
			else
				this->_request.append("Connection: close\r\n");
			req.append("Set-Cookie: session_id=");
			req.append(createCookieId());
			req.append("; HttpOnly\r\n");
			std::string file = getFile(this->_path);
			req.append(getContentType(this->_path));
			req.append(this->_length);
			req.append("\r\n\r\n");
			req.append(file);
			write(fd, req.c_str(), req.length());
		}
		else
		{
			this->sendClient(fd, 200, this->_path);
		}
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
	(void)rules;
	std::string path = this->_path;
	if (std::remove(path.c_str()) == 0)
	{
		sendClient(fd, 204, "");
	}
	else
	{
		sendClient(fd, 404, "./data/ressources/empty.txt");
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

