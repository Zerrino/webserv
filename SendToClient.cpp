/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SendToClient.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 22:20:30 by zerrino           #+#    #+#             */
/*   Updated: 2024/11/09 07:55:23 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SendToClient.hpp"

SendToClient::SendToClient()
{
	std::cout << "SendToClient created!" << std::endl;
}

SendToClient::~SendToClient()
{
}

SendToClient::SendToClient(const SendToClient& cp)
{
 	*this = cp;
}

SendToClient& SendToClient::operator= (const SendToClient& cp)
{
 	if (this != &cp)
		*this = cp;
 	return (*this);
}

void	SendToClient::SayHey()
{
	std::cout << "Hey!" << std::endl;
}

void	SendToClient::requestOne(int request)
{
	this->_request = "";
	switch (request)
	{
	case 100 :
		this->_request = "HTTP/1.1 100 Continue\r\n";
		break;
	default:
		throw std::runtime_error("error type not handled");
		break;
	}
	this->_request.append(this->getDate());
}

std::string	SendToClient::requestTwo(int request, std::string path)
{
	std::string file;
	std::string ret;
	this->_request = "";
	this->_length = "";
	switch (request)
	{
	case 200:
		this->_request = "HTTP/1.1 200 OK\r\n";
		this->_request.append(this->getDate());
		this->_request.append(this->getContentType(path));
		file = getFile(path);
		this->_request.append(this->_length);
		this->_request.append("\r\n");
		this->_request.append(file);
		break;
	case 201:
		ret = "";
		break;
	case 202:
		ret = "";
		break;
	case 204:
		ret = "";
		break;
	case 206:
		ret = "";
		break;
	default:
		throw std::runtime_error("error type not handled");
		break;
	}
	return (this->_request);
}

void	SendToClient::requestThree(int request, std::string path)
{

}

void	SendToClient::requestFour(int request, std::string path)
{

}

void	SendToClient::requestFive(int request, std::string path)
{

}

std::string	SendToClient::getDate()
{
	time_t timestamp;
	char buffer[31];
	time(&timestamp);
	struct tm *gmt = std::gmtime(&timestamp);
	std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT\r\n", gmt);
	std::string str(buffer);
	std::string s = "Date: ";
	s.append(str);
	return (s);
}

std::string SendToClient::getContentType(std::string path)
{
	const std::map<std::string, std::string>& contentTypes = getContentTypesMap();
	std::string str;
	std::size_t pos = path.find_last_of('.');

	if (pos != std::string::npos)
		str = path.substr(pos + 1);
	else
		throw std::runtime_error("file has no extension");
	std::map<std::string, std::string>::const_iterator it = contentTypes.find(str);
	if (it != contentTypes.end())
	{
		str = "Content-Type: ";
		str.append(it->second);
		str.append("\r\n");
		return (str);
	}
	throw std::runtime_error("unsupported type of file");
}

std::string	SendToClient::getFile(std::string path)
{
	std::stringstream ss;
	std::string str;
	std::ifstream file(path.c_str());
	if (!file.is_open())
		throw std::runtime_error("couldn't open the file");
	std::getline(file, str, '\0');
	this->_length = "Content-Length: ";
	ss << str.size();
	this->_length.append((ss.str()));
	this->_length.append("\r\n");
	file.close();
	return (str);
}

const std::map<std::string, std::string>& SendToClient::getContentTypesMap()
{
	static std::map<std::string, std::string> contentTypes;
	if (contentTypes.empty())
	{
		contentTypes["html"] = "text/html";
		contentTypes["htm"] = "text/html";
		contentTypes["txt"] = "text/plain";
		contentTypes["csv"] = "text/csv";
		contentTypes["css"] = "text/css";
		contentTypes["js"] = "application/javascript";
		contentTypes["json"] = "application/json";
		contentTypes["xml"] = "application/xml";
		contentTypes["yaml"] = "application/x-yaml";
		contentTypes["jpeg"] = "image/jpeg";
		contentTypes["jpg"] = "image/jpeg";
		contentTypes["png"] = "image/png";
		contentTypes["gif"] = "image/gif";
		contentTypes["bmp"] = "image/bmp";
		contentTypes["webp"] = "image/webp";
		contentTypes["svg"] = "image/svg+xml";
		contentTypes["ico"] = "image/vnd.microsoft.icon";
		contentTypes["mp4"] = "video/mp4";
		contentTypes["mpeg"] = "video/mpeg";
		contentTypes["avi"] = "video/x-msvideo";
		contentTypes["webm"] = "video/webm";
		contentTypes["mp3"] = "audio/mpeg";
		contentTypes["m4a"] = "audio/mp4";
		contentTypes["pdf"] = "application/pdf";
	}
	return (contentTypes);
}
