/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 05:56:31 by marvin            #+#    #+#             */
/*   Updated: 2024/11/15 05:56:31 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Request.hpp"

Request::Request()
{
}

Request::~Request()
{
}

Request::Request(const Request& cp)
{
	*this = cp;
}

Request& Request::operator= (const Request& cp)
{
	if (this != &cp)
		*this = cp;
	return (*this);
}

void	Request::printMap(std::map<std::string, std::string> myMap)
{
	if (myMap.size() == 0)
		return;
	std::cout << "-------------------New Request--------------------" << std::endl;
	for (std::map<std::string, std::string>::iterator it = myMap.begin(); it != myMap.end(); ++it)
	{
		if (it->first != "Content")
			std::cout << "[\"" << it->first << "\"] = \"" << it->second << "\"" << std::endl;
	}
	std::cout << "--------------------------------------------------" << std::endl << std::endl;
}

std::map<std::string, std::string> Request::parseRequest(std::string request)
{
	Cookie cook;
	std::map<std::string, std::string> req;
	std::string boundary;
	std::string	tmp;
	std::string link;
	std::size_t pos0;
	std::size_t pos1;
	if (request.length() < 5) return req;
	pos0 = request.find(' ');
	if (pos0 == std::string::npos) return req;
	tmp = request.substr(pos0 + 1);
	pos1 = tmp.find(' ');
	if (pos1 == std::string::npos) return req;
	link = tmp.substr(0, pos1);
	pos1 = link.find('?');
	if (pos1 != std::string::npos)
	{
		req = cook.getRequestData(link.substr(pos1 + 1));
		link = link.substr(0, pos1);
	}
	req[request.substr(0, pos0)] = link;
	req["Cookie_ID"] = cook.isCookied(request);
	pos0 = request.find("WebKitFormBoundary");
	tmp = "";
	if (pos0 != std::string::npos)
	{
		req["end"] = "false";
		boundary = request.substr(pos0);
		pos0 = boundary.find("\r\n");
		tmp = request.substr(pos0);
		boundary = boundary.substr(0, pos0);
		pos0 = tmp.find(boundary);
		tmp = tmp.substr(pos0 + boundary.length());
		pos0 = tmp.find(boundary);
		link = tmp;
		tmp = tmp.substr(pos0);
		pos0 = tmp.find("\r\n\r\n");
		link = tmp.substr(0, pos0);
		tmp = tmp.substr(pos0 + 4);
		pos0 = tmp.find(boundary);
		if (pos0 != std::string::npos)
		{
			tmp = tmp.substr(0, pos0 - 8);
			req["end"] = "true";
		}
		pos0 = request.find("\r\n\r\n");
		request = request.substr(0, pos0);
		request.append(link);
		req["Content"] = tmp;
	}
	else
	{
		pos0 = request.find("\r\n\r\n");
		req["Content"] = "";
		if (pos0 != std::string::npos)
		{
			req["Content"] = request.substr(pos0 + 4);
		}
	}
	pos0 = request.find("\r\n");
	if (pos0 == std::string::npos) return req;
	request = request.substr(pos0 + 2);

	pos0 = request.find("\r\n");
	while (pos0 != std::string::npos)
	{
		tmp = request.substr(0, pos0);
		pos1 = tmp.find(": ");
		if (pos1 == std::string::npos) return req;
		req[tmp.substr(0, pos1)] = tmp.substr(pos1 + 2);
		request = request.substr(pos0 + 2);
		pos0 = request.find("\r\n");
	}
	pos1 = request.find(": ");
	if (pos1 == std::string::npos) return req;
	req[request.substr(0, pos1)] = request.substr(pos1 + 2);
	return (req);
}

std::map<std::string, std::string> Request::mergeMap(std::map<std::string, std::string> myMap, std::map<std::string, std::string> mySecondMap)
{
	for (std::map<std::string, std::string>::iterator it = mySecondMap.begin(); it != mySecondMap.end(); ++it)
	{
		myMap[it->first] = it->second;
	}
	return myMap;
}

void	Request::parseContent(std::map<std::string, std::string> &myMap)
{
	std::string content = myMap["Content"];
	if (content.length() == 0)
		return;
	content.erase(std::remove(content.begin(), content.end(), '{'), content.end());
	content.erase(std::remove(content.begin(), content.end(), '}'), content.end());

	std::istringstream ss(content);
	std::string pair;
	while (std::getline(ss, pair, ','))
	{
		size_t delimiterPos = pair.find(':');
		if (delimiterPos != std::string::npos)
		{
			std::string key = pair.substr(0, delimiterPos);
			std::string value = pair.substr(delimiterPos + 1);

			key.erase(std::remove(key.begin(), key.end(), '\"'), key.end());
			value.erase(std::remove(value.begin(), value.end(), '\"'), value.end());
			myMap[key] = value;
		}
	}
}

std::map<std::string, std::string> Request::parseInd(std::string key, std::string content)
{
	std::map<std::string, std::string> myMap;
	std::size_t	pos0;
	std::string str;
	std::string tmp;

	str = content;
	//std::cout << "key " << key << content << std::endl;
	pos0 = str.find("; ");
	if (pos0 != std::string::npos)
	{
		myMap[key] = content.substr(0, pos0);
		while (pos0 != std::string::npos)
		{
			tmp = str.substr(0, pos0);
			str = str.substr(pos0 + 2);
			pos0 = tmp.find("=");
			if (pos0 != std::string::npos)
				myMap[tmp.substr(0, pos0)] = tmp.substr(pos0 + 1);
			pos0 = str.find("; ");
		}
	}
	return myMap;
}

std::map<std::string, std::string> Request::reformat(std::map<std::string, std::string> myMap)
{
	std::size_t	pos0;
	std::string str;
	std::string tmp;
	for (std::map<std::string, std::string>::iterator it = myMap.begin(); it != myMap.end(); ++it)
	{
		str = it->second;
		pos0 = str.find("; ");
		if (pos0 != std::string::npos)
		{
			myMap[it->first] = (it->second).substr(0, pos0);
			while (pos0 != std::string::npos)
			{
				tmp = str.substr(0, pos0);
				str = str.substr(pos0 + 2);
				pos0 = tmp.find("=");
				if (pos0 != std::string::npos)
					myMap[tmp.substr(0, pos0)] = tmp.substr(pos0 + 1);
				pos0 = str.find("; ");
			}
			pos0 = str.find("=");
			if (pos0 != std::string::npos)
				myMap[str.substr(0, pos0)] = str.substr(pos0 + 1);
		}
	}
	return myMap;
}

std::map<std::string, std::string> Request::reParseRequest(std::map<std::string, std::string> myMap)
{
	std::size_t pos0;
	std::size_t pos1;
	std::string boundary;
	std::string tmp;
	if (myMap["Content"].length() == 0)
		return myMap;
	pos0 = myMap["Content"].find("\r\n");
	if (pos0 == std::string::npos) return myMap;
	boundary = myMap["Content"].substr(0, pos0);
	pos1 = myMap["Content"].find("\r\n\r\n");
	if (pos1 == std::string::npos) return myMap;
	tmp = myMap["Content"].substr(0, pos1);
	myMap["Content"] = myMap["Content"].substr(pos1 + 4);
	pos1 = myMap["Content"].find(boundary);
	if (pos1 == std::string::npos) return myMap;
	myMap["Content"] = myMap["Content"].substr(0, pos1);
	pos0 = tmp.find("\r\n");
	if (pos0 == std::string::npos) return myMap;
	tmp = tmp.substr(pos0 + 2);
	boundary = tmp;
	pos0 = tmp.find("\r\n");
	while (pos0 != std::string::npos)
	{
		boundary = tmp.substr(0, pos0);
		pos1 = boundary.find(": ");
		if (pos1 == std::string::npos) return myMap;
		myMap[boundary.substr(0, pos1)] = boundary.substr(pos1 + 2);
		tmp = tmp.substr(pos0 + 2);
		pos0 = tmp.find("\r\n");
	}
	pos1 = tmp.find(": ");
	if (pos1 == std::string::npos) return myMap;
	myMap[tmp.substr(0, pos1)] = tmp.substr(pos1 + 2);
	return myMap;
}


std::string Request::urlParsing(std::vector<std::string> listLocation, std::string url)
{
	std::size_t pos;

	_pathUrl = url;
	pos = url.find("/");
	if (pos == std::string::npos)
		return "none";
	if (url.length() == 1)
	{
		_pathUrl = "";
		return ("/");
	}
	while (true)
	{
		for (std::size_t i = 0; i < listLocation.size(); i++)
		{
			if (listLocation[i] == url)
			{
				_pathUrl = _pathUrl.substr(listLocation[i].length());
				return listLocation[i];
			}
		}
		pos = url.rfind("/");
		if (pos == std::string::npos)
			return "none";
		if (url.substr(0, pos + 1) == "/")
		{
			_pathUrl = "";
			return ("/");
		}
		url = url.substr(0, pos);
	}
	return url;
}
