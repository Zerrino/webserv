/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SendToClient.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 22:20:30 by zerrino           #+#    #+#             */
/*   Updated: 2024/12/18 12:08:35 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/SendToClient.hpp"

SendToClient::SendToClient()
{
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

std::string	SendToClient::requestOne(int request, bool keepAlive)
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
	if (keepAlive)
		this->_request.append("Connection: keep-alive\r\n");
	else
		this->_request.append("Connection: close\r\n");
	//this->_request.append(this->getDate());
	this->_request.append("\r\n");
	return (this->_request);
}

std::string	SendToClient::requestTwo(int request, std::string path, bool keepAlive)
{
	std::string file;
	this->_request = "";
	this->_length = "";
	switch (request)
	{
	case 200:
		this->_request = "HTTP/1.1 200 OK\r\n";
		if (keepAlive)
			this->_request.append("Connection: keep-alive\r\n");
		else
			this->_request.append("Connection: close\r\n");
		this->_request.append(this->getContentType(path));
		file = getFile(path);
		this->_request.append(this->_length);
		this->_request.append("\r\n");
		this->_request.append(file);
		break;
	case 201:
		this->_request = "HTTP/1.1 201 Created\r\n";
		if (keepAlive)
			this->_request.append("Connection: keep-alive\r\n");
		else
			this->_request.append("Connection: close\r\n");
		this->_request.append(this->getContentType(path));
		this->_request.append("Location : ");
		this->_request.append(_locationCreate);
		this->_request.append("\r\n");
		this->_request.append(file);
		// TOUJOURS A FINIR ICI ENFIN SI ON UTILISE CE GENRE DE REQUETES
		break;
	case 204:
		this->_request = "HTTP/1.1 204 No Content\r\n";
		if (keepAlive)
			this->_request.append("Connection: keep-alive\r\n");
		else
			this->_request.append("Connection: close\r\n");
		this->_request.append("\r\n");
		break;
	default:
		throw std::runtime_error("error type not handled");
		break;
	}
	return (this->_request);
}

std::string	SendToClient::requestThree(int request, std::string path, bool keepAlive)
{
	this->_request = "";
	this->_length = "";
	switch (request)
	{
	case 301:
		this->_request = "HTTP/1.1 301 Moved Permanently\r\n";
		break;
	case 302:
		this->_request = "HTTP/1.1 302 Found\r\n";
		break;
	case 303:
		this->_request = "HTTP/1.1 303 See Other\r\n";
		break;
	case 304:
		this->_request = "HTTP/1.1 304 Not Modified\r\n";
		if (keepAlive)
			this->_request.append("Connection: keep-alive\r\n");
		else
			this->_request.append("Connection: close\r\n");
		this->_request.append("\r\nContent-Length: 0\r\n\r\n");
		break;
	default:
		throw std::runtime_error("error type not handled");
		break;
	}
	if (request != 304)
	{
		if (keepAlive)
			this->_request.append("Connection: keep-alive\r\n");
		else
			this->_request.append("Connection: close\r\n");
		this->_request.append("Location: ");
		this->_request.append(path);
		this->_request.append("\r\nContent-Length: 0\r\n\r\n");
	}
	return (this->_request);
}

std::string	SendToClient::requestFour(int request, std::string path, bool keepAlive)
{
	std::string file;
	this->_request = "";
	this->_length = "";
	switch (request)
	{
	case 403:
		this->_request = "HTTP/1.1 403 Forbidden\r\n";
		break;
	case 404:
		this->_request = "HTTP/1.1 404 Not Found\r\n";
		break;
	case 405:
		this->_request = "HTTP/1.1 405 Method Not Allowed\r\n";
		break;
	case 413:
		this->_request = "HTTP/1.1 413 Payload Too Large\r\n";
		break;
	default:
		throw std::runtime_error("error type not handled");
		break;
	}
	if (keepAlive)
		this->_request.append("Connection: keep-alive\r\n");
	else
		this->_request.append("Connection: close\r\n");
	this->_request.append(this->getContentType(path));
	file = getFile(path);
	this->_request.append(this->_length);
	this->_request.append("\r\n");
	this->_request.append(file);
	return (this->_request);
}

std::string	SendToClient::requestFive(int request, std::string path, bool keepAlive)
{
	std::string file;
	this->_request = "";
	this->_length = "";
	switch (request)
	{
	case 500:
		this->_request = "HTTP/1.1 500 Internal Server Error\r\n";
		break;
	case 501:
		this->_request = "HTTP/1.1 501 Not Implemented\r\n";
		break;
	case 502:
		this->_request = "HTTP/1.1 502 Bad Gateway\r\n";
		break;
	default:
		throw std::runtime_error("error type not handled");
		break;
	}
	if (keepAlive)
		this->_request.append("Connection: keep-alive\r\n");
	else
		this->_request.append("Connection: close\r\n");
	this->_request.append(this->getContentType(path));
	file = getFile(path);
	this->_request.append(this->_length);
	this->_request.append("\r\n");
	this->_request.append(file);
	return (this->_request);
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
		return ("");
		//throw std::runtime_error("file has no extension");
	std::map<std::string, std::string>::const_iterator it = contentTypes.find(str);
	if (it != contentTypes.end())
	{
		str = "Content-Type: ";
		str.append(it->second);
		str.append("\r\n");
		return (str);
	}
	str = "Content-Type: ";
	str.append("text/plain");
	str.append("\r\n");
	return (str);
	throw std::runtime_error("unsupported type of file");
}

std::string	SendToClient::getFile(std::string path)
{
	std::ifstream file(path.c_str(), std::ios::binary);
	std::stringstream ss;
	std::stringstream len_ss;
	std::string file_data;
	if (!file.is_open())
		throw std::runtime_error("couldn't open the file");
	ss << file.rdbuf();
	file_data = ss.str();
	file.close();
	len_ss << file_data.size();
	this->_length = "Content-Length: ";
	this->_length.append((len_ss.str()));
	this->_length.append("\r\n");
	return (file_data);
}

const std::map<std::string, std::string>& SendToClient::getContentTypesMap()
{
	static std::map<std::string, std::string> contentTypes;
	if (contentTypes.empty())
	{
		contentTypes["html"] = "text/html";
		contentTypes["bad_extension"] = "text/html";
		contentTypes["pouic"] = "text/html";
		contentTypes["bla"] = "text/html";
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
