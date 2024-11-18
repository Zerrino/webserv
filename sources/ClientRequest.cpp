/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 05:18:28 by Zerrino           #+#    #+#             */
/*   Updated: 2024/11/18 07:39:08 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ClientRequest.hpp"

ClientRequest::ClientRequest(std::vector<int> fdSocket)
	:	SendToClient(), _fdSocket(fdSocket)
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

void	ClientRequest::pollRequest()
{
	int	ret = poll(this->_fds.data(), this->_fds.size(), 30000);
	if (ret == -1)
		throw std::runtime_error("poll failed");
}

void	ClientRequest::pollExecute()
{
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
				int	flag;

				std::string str = this->get_clientInfo(this->_fds[i].fd);
				//std::cout << this->_clientInfo << std::endl;
				//printMap(reformat(parseRequest(this->_clientInfo)));
				//printMap(parseRequest(this->_clientInfo));
				//std::size_t pos;
				std::string rest;
				std::string cook;


				if (_clMap.find("Cookie") != _clMap.end())
				{
					//std::cout << "Cookie is " << cook << std::endl;
					flag = 0;
				}
				else
				{
					std::cout << "No Cookies..." << std::endl;
					flag = 1;
				}
				// STR = le PATH de la requete

				std::string PATH_ABS = "./data";
				//std::cout << PATH_ABS << std::endl;
				//cook = isCookied(this->_clientInfo);
				//std::cout << "cookie : " << cook << std::endl;
				if (_clMap.find("GET") != _clMap.end())
				{
					PATH_ABS.append(_clMap["GET"]);
					if (_clMap["GET"] == "/")
					{
						std::string file_index = "index.html";
						PATH_ABS.append(file_index);
						if (!flag)
							this->sendClient(this->_fds[i].fd, 200, PATH_ABS);
						else
						{
							std::string req = "HTTP/1.1 200 OK\r\n";
							req.append(getDate());
							req.append("Set-Cookie: session_id=");
							req.append(createCookieId());
							req.append("; HttpOnly\r\n");
							std::string file = getFile(PATH_ABS);
							req.append(getContentType(PATH_ABS));
							req.append(this->_length);
							req.append("\r\n\r\n");
							req.append(file);
							write(this->_fds[i].fd, req.c_str(), req.length());
							flag = 0;
						}
					}
					else if (_clMap.find("email") != _clMap.end())
					{
						cook = _clMap["Cookie_ID"];
						//std::cout << isRegister(getRequestData(rest)) << std::endl;
						//std::cout << "cookie : " << cook << std::endl;
						if ((isRegister(_clMap)) && (cook.length() > 0))
						{
							cookiedUpdate("login", "true", cook);
							this->sendClient(this->_fds[i].fd, 200, "./data/src/dashboard.html");
						}
						else
						{
							cookiedUpdate("login", "false", cook);
							this->sendClient(this->_fds[i].fd, 302, "/"); // Ici si il a pas mis un bon mdp ?
						}
					}
					else // SI aucune informations en plus
					{
						std::string content = getContentType(PATH_ABS);
						cook = _clMap["Cookie_ID"];
						//std::cout << "Cookie " << cook << " is " << isCookies("login", "true", cook) << std::endl;
						//std::cout << PATH_ABS << std::endl;
						if ((content != "Content-Type: text/html\r\n") || (isCookies("login", "true", cook) == 1))
							this->sendClient(this->_fds[i].fd, 200, PATH_ABS);
						//else
						//	this->sendClient(this->_fds[i].fd, 302, "/"); // redirige si il essaye d'aller autre pars que le debut .html et est pas login
					}
				}
				close(this->_fds[i].fd);
				this->_fds.erase(this->_fds.begin() + i);
				--i;
			}
		}
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

std::string	ClientRequest::get_clientInfo(int fd)
{
	char buffer[4096] = {0};
	std::size_t len;
	std::size_t pos;
	std::string str;
	std::string res;
	int	i = 0;
	str = "";
	res = "";
	std::cout << "Hey" << std::endl;
	while (true)
	{
		len = read(fd, buffer, sizeof(buffer));
		//std::cout << len << std::endl;
		if (i == 0)
		{
			str.append(buffer, sizeof(buffer));
			res = str;
			_clMap = reformat(parseRequest(str));
			printMap(_clMap);
		}
		if ((_clMap.find("POST") != _clMap.end()) && _clMap["end"] == "false")
		{
			if (!_clMap["filename"].empty() && _clMap["filename"][0] == '"' && _clMap["filename"][_clMap["filename"].size() - 1] == '"')
			{
				_clMap["filename"].erase(0, 1);
				_clMap["filename"].erase(_clMap["filename"].size() - 1);
			}
			std::ofstream outfile(_clMap["filename"].c_str(), std::ios::out | std::ios::binary | std::ios::app);
			if (!outfile)
				throw std::runtime_error("unable to create file");
			str = "";
			if (i == 0)
				str = _clMap["Content"];
			else
				str.append(buffer, len);
			pos = str.find("------WebKitFormBoundary");
			if (pos != std::string::npos)
			{
				str = str.substr(0, pos);
				std::cout << str << std::endl;
			}
			outfile.write(str.c_str(), str.size());
			if (pos != std::string::npos)
				break;
		}
		if (len != sizeof(buffer))
		{
			if (_clMap.find("POST") == _clMap.end())
				break;
		}
		i++;
	}
	std::cout << "end!" << std::endl;
	std::cout << i << std::endl;
	this->_clientInfo = res;
	return (res);
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

void	ClientRequest::sendClient(int fd, int request, std::string path)
{
	std::string	str = "";
	if ((request >= 100) && (request < 200))
		str = this->requestOne(request);
	else if ((request >= 200) && (request < 300))
		str = this->requestTwo(request, path);
	else if ((request >= 300) && (request < 400))
		str = this->requestThree(request, path);
	else if ((request >= 400) && (request < 500))
		str = this->requestFour(request, path);
	else if ((request >= 500) && (request < 600))
		str = this->requestFive(request, path);
	write(fd, str.c_str(), str.length());
}















