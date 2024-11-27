/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 05:18:28 by Zerrino           #+#    #+#             */
/*   Updated: 2024/11/24 22:09:31 by marvin           ###   ########.fr       */
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
				std::string str = this->get_clientInfo(this->_fds[i].fd);
				std::string rest;
				if (_clMap.find("GET") != _clMap.end())
				{
					handlingGET(i);
				}
				else if (_clMap.find("POST") != _clMap.end())
				{
					handlingPOST(i);
				}
				else if (_clMap.find("PUT") != _clMap.end())
				{
					handlingPUT(i);
				}
				else if (_clMap.find("DELETE") != _clMap.end())
				{
					handlingDELETE(i);
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
		if (!headersParsed)
		{
			requestData.append(buffer, len);
			std::size_t pos = requestData.find("\r\n\r\n");
			if (pos != std::string::npos)
			{
				headersParsed = true;
				std::string headers = requestData.substr(0, pos + 4);
				_clMap = reformat(parseRequest(headers));
				parseContent(_clMap);
				std::stringstream ss(_clMap["Content-Length"]);
				ss >> contentLength;
				std::string contentData = requestData.substr(pos + 4);
				contentRead = contentData.size();
				_clMap["Content"].resize(contentLength);
				_clMap["Content"] = contentData;
				if (contentRead >= contentLength)
					break;
			}
		}
		else
		{
			std::size_t i = 0;
			while (i < len)
			{
				_clMap["Content"][i + contentRead] = buffer[i];
				i++;
			}
			contentRead += len;
			if (contentRead >= contentLength)
				break;
		}
	}
	printMap(_clMap);
	this->_clientInfo = requestData;
	return requestData;
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




//		void	handlingGET();
//		void	handlingPUT();
//		void	handlingDELETE();
//		void	handlingPOST();

void	ClientRequest::handlingGET(int i)
{
	std::string cook;
	std::string PATH_ABS = "./data";
	PATH_ABS.append(_clMap["GET"]);
	if (_clMap["GET"] == "/")
	{
		std::string file_index = "index.html";
		PATH_ABS.append(file_index);
		if (_clMap.find("Cookie") != _clMap.end())
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
		}
	}
	else // SI aucune informations en plus
	{
		std::string content = getContentType(PATH_ABS);
		cook = _clMap["Cookie_ID"];
		this->sendClient(this->_fds[i].fd, 200, PATH_ABS);
	}
}
void	ClientRequest::handlingPUT(int i)
{
	std::string path = "./data";
	int	number;
	path.append(_clMap["PUT"]);
	std::ofstream file(path.c_str());
	if (!file.is_open())
		sendClient(this->_fds[i].fd, 404, "./data/ressources/responses/errors/errorkey.txt");
	std::istringstream iss(_clMap["Content-Length"]);
	iss >> number;
	file.write(_clMap["Content"].c_str(), number);
	file.close();
	sendClient(this->_fds[i].fd, 204, "");
}
void	ClientRequest::handlingDELETE(int i)
{
	std::string path = "./data";
	path.append(_clMap["PUT"]);
	if (std::remove(path.c_str()) == 0)
	{
		sendClient(this->_fds[i].fd, 204, "");
	}
	else
	{
		sendClient(this->_fds[i].fd, 404, "./data/ressources/responses/errors/errorkey.txt");
	}
}
void	ClientRequest::handlingPOST(int i)
{
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
				sendClient(this->_fds[i].fd, 204, "");
			}
			else
			{
				cookiedUpdate("login", "false", path_cookies.append(_clMap["Cookie_ID"]));
				if (resultCook == -1)
					sendClient(this->_fds[i].fd, 404, "./data/ressources/responses/errors/errorkey.txt");
				else
					sendClient(this->_fds[i].fd, 404, "./data/ressources/responses/errors/errorval.txt");
			}
		}
		else if (_clMap["action"].find("modify") != std::string::npos)
		{
			if (isCookies(_clMap["email"], "a", "database/profiles.txt") == 0)
			{
				cookiedUpdate(_clMap["email"], _clMap["password"], "database/profiles.txt");
				sendClient(this->_fds[i].fd, 204, "");
			}
			else
				sendClient(this->_fds[i].fd, 404, "./data/ressources/empty.txt");
		}
		else if (_clMap["action"].find("create") != std::string::npos)
		{
			if (isCookies(_clMap["email"], "", "database/profiles.txt") == -1)
			{
				cookiedUpdate(_clMap["email"], _clMap["password"], "database/profiles.txt");
				sendClient(this->_fds[i].fd, 204, "");
			}
			else
				sendClient(this->_fds[i].fd, 404, "./data/ressources/empty.txt");
		}
	}
}

