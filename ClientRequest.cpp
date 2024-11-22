/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: root <root@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 05:18:28 by Zerrino           #+#    #+#             */
/*   Updated: 2024/11/22 12:04:06 by root             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientRequest.hpp"

std::string getTestRequestGETPYTHON() {
    return "POST /hello.py HTTP/1.1\n"
           "Host: example.com\n"
           "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.75 Safari/537.36\n"
           "Content-Type: application/x-www-form-urlencoded\n"
           "Content-Length: 28\n"
           "Connection: keep-alive\n\n"
           "body=alice";
}

std::string getTestRequestPOSTPHP(std::string info) {
    return "POST / HTTP/1.1\n"
			"Host: localhost:5000\n"
			"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:132.0) Gecko/20100101 Firefox/132.0\n"
			"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\n"
			"Accept-Language: fr,fr-FR;q=0.8,en-US;q=0.5,en;q=0.3\n"
			"Accept-Encoding: gzip, deflate, br, zstd\n"
			"Content-Type: application/x-www-form-urlencoded\n"
			"Content-Length: 8\n"
			"Origin: http://localhost:5000\n"
			"DNT: 1\n"
			"Connection: keep-alive\n"
			"Referer: http://localhost:5000/\n"
			"Cookie: PHPSESSID=0op581bmb0jflctvek4uidgqhb\n"
			"Upgrade-Insecure-Requests: 1\n"
			"Sec-Fetch-Dest: document\n"
			"Sec-Fetch-Mode: navigate\n"
			"Sec-Fetch-Site: same-origin\n"
			"Sec-Fetch-User: ?1\n"
			"Priority: u=0, i\n\n"
			"name=hey";
}

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
	int	ret = poll(this->_fds.data(), this->_fds.size(), 3000);
	if (ret == -1)
		throw std::runtime_error("poll failed");
}
				bool isPHPFile(const std::string& path) {
					return path.size() >= 4 && path.substr(path.size() - 4) == ".php";
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
				int	request_done;
				request_done = 0;
				this->get_clientInfo(this->_fds[i].fd);
					
				std::string str = this->_clientInfo;
				std::size_t pos = str.find('/');
				std::string rest;
				std::string cook;

				if (pos != std::string::npos && str.length() >= pos)
					str = str.substr(pos);
				pos = str.find(' ');
				if (pos != std::string::npos && str.length() >= pos)
					str = str.substr(0, pos);
				pos = str.find('?');
				if (pos != std::string::npos && str.length() >= pos)
				{
					request_done = 1;
					rest = str.substr(pos + 1);
					str = str.substr(0, pos);
				}
				if (this->_clientInfo.length() > 10)
				{
					cook = isCookied(this->_clientInfo);
					if (cook.length() != 0) flag = 0;
					else flag = 1;
				}

				std::string PATH_ABS = "/var/www/html/data";
				PATH_ABS.append(str);
				std::cout << PATH_ABS << std::endl;
				if (str == "")
				{}
				else if (str == "/")
				{
					std::string file_index = "welcome.php";
					PATH_ABS.append("src/");
					PATH_ABS.append(file_index);
					std::cout << PATH_ABS << std::endl;
					
					int cgi = CGIchecker(this->_clientInfo, PATH_ABS, this->_fds[i].fd);
					if(cgi == 2)
						std::cerr << "File is not php or py and can't be handled by the CGI." << std::endl;
					else if (cgi == 1)
						std::cerr << "Error while executing CGI request." << std::endl;
				}
				else if (request_done)
				{
					cook = isCookied(this->_clientInfo);
					//std::cout << isRegister(getRequestData(rest)) << std::endl;
					//std::cout << "cookie : " << cook << std::endl;
					if ((isRegister(getRequestData(rest))) && (cook.length() > 0))
					{
						cookiedUpdate("login", "true", cook);
						this->sendClient(this->_fds[i].fd, 200, "./data/src/dashboard.html");
					} else {
						cookiedUpdate("login", "false", cook);
						this->sendClient(this->_fds[i].fd, 302, "/"); // Ici si il a pas mis un bon mdp ?
					}
				}
				else // SI aucune informations en plus
				{
					std::string content = getContentType(PATH_ABS);
					cook = isCookied(this->_clientInfo);
					//std::cout << "Cookie " << cook << " is " << isCookies("login", "true", cook) << std::endl;
					// std::cout << PATH_ABS << std::endl;
					// if ((content != "Content-Type: text/html\r\n") || (isCookies("login", "true", cook) == 1))
						this->sendClient(this->_fds[i].fd, 200, PATH_ABS);
					// else
					// 	this->sendClient(this->_fds[i].fd, 302, "/"); // redirige si il essaye d'aller autre pars que le debut .html et est pas login
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
	char buffer[1024] = {0};
	read(fd, buffer, 1024);
	std::string str(buffer);
	this->_clientInfo = str;
	return (this->_clientInfo);
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















