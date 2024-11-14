/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cookie.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 04:06:02 by zerrino           #+#    #+#             */
/*   Updated: 2024/11/14 21:42:04 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Cookie.hpp"

Cookie::Cookie()
{
}

Cookie::~Cookie()
{
}

Cookie::Cookie(const Cookie& cp)
{
	*this = cp;
}

Cookie& Cookie::operator= (const Cookie& cp)
{
	if (this != &cp)
		*this = cp;
	return (*this);
}

std::string Cookie::createCookieId()
{
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";


	std::string tmp;
	std::string path = "./data/ressources/cookies/";
	tmp.reserve(8);
	for (size_t i = 0; i < 8; i++)
	{
		tmp += alphanum[rand() % (sizeof(alphanum) - 1)];
	}
	path.append(tmp);
	std::ifstream file(path.c_str());
	if (file.is_open())
		throw std::runtime_error("Cookie already exist");
	std::ofstream new_file(path.c_str());
	if (!new_file)
		throw std::runtime_error("unable to create file");
	new_file << "login=false&" << std::endl;
	file.close();
	new_file.close();
	return (tmp);
}

std::string Cookie::isCookied(std::string path)
{
	std::size_t pos = path.find("Cookie");
	if (pos == std::string::npos)
		return "";
	path = path.substr(pos);
	pos = path.find("session_id=");
	if (pos != std::string::npos)
	{
		size_t start = pos + 11;
		size_t end = path.find('\n', start);
		if (end == std::string::npos)
			end = path.length();
		return path.substr(start, end - (start + 1));
	}
	return "";
}

std::map<std::string, std::string> Cookie::getRequestData(std::string data)
{
	std::map<std::string, std::string> myData;
	std::string tmp;
	std::string myKey;
	std::string myVal;
	std::size_t pos;

	pos = data.find('&');
	while (pos != std::string::npos)
	{
		tmp = data.substr(0, pos);
		pos = tmp.find('=');
		if (pos != std::string::npos)
		{
			myKey = tmp.substr(0, pos);
			myVal = tmp.substr(pos + 1);
			myData[myKey] = myVal;
		}
		pos = data.find('&');
		data = data.substr(pos + 1);
	}
	this->_requestData = myData;
	return (myData);
}

int	Cookie::isCookies(std::string myKey, std::string myVal, std::string CookiedID)
{
	std::string	file_data;
	std::stringstream ss;
	std::string path = "./data/ressources/cookies/";
	path.append(CookiedID);
	std::ifstream file(path.c_str());
	if (!file.is_open())
		throw std::runtime_error("cookie doesn't exist");
	ss << file.rdbuf();
	file_data = ss.str();
	file.close();
	std::map<std::string, std::string> myData = getRequestData(file_data);
	if (myData[myKey] == myVal)
		return (1);
	return (0);
}
void Cookie::cookiedUpdate(std::string myKey, std::string myVal, std::string CookiedID)
{
	std::string	file_data;
	std::stringstream ss;
	std::string path = "./data/ressources/cookies/";
	path.append(CookiedID);
	std::ifstream file(path.c_str());
	if (!file.is_open())
		throw std::runtime_error("cookie doesn't exist");
	ss << file.rdbuf();
	file_data = ss.str();
	file.close();
	std::map<std::string, std::string> myData = getRequestData(file_data);
	myData[myKey] = myVal;
	file_data = "";
	for (std::map<std::string, std::string>::iterator it = myData.begin(); it != myData.end(); ++it)
	{
		file_data.append(it->first);
		file_data.append("=");
		file_data.append(it->second);
		file_data.append("&");
	}
	std::ofstream replace_file(path.c_str(), std::ios::out | std::ios::trunc);
	if (!replace_file)
		throw std::runtime_error("error opening file");
	replace_file.write(file_data.c_str(), file_data.size());
	replace_file.close();
}

int	Cookie::isRegister(std::map<std::string, std::string> myData)
{
	std::string email;
	std::string password;
	std::string	file_data;
	std::stringstream ss;
	std::size_t pos;

	if ((myData.find("email") != myData.end()) && (myData.find("password") != myData.end()))
	{
		email = myData["email"];
		password = myData["password"];
	}
	else
		return 0;
	std::ifstream file("./data/ressources/database/profiles.txt");
	if (!file.is_open())
		return 0;
	ss << file.rdbuf();
	file_data = ss.str();
	file.close();
	pos = file_data.find(email);
	if (pos == std::string::npos)
		return 0;
	file_data = file_data.substr(pos);
	pos = file_data.find('\n');
	if (pos != std::string::npos)
		file_data = file_data.substr(0, pos);
	pos = file_data.find(':');
	if (pos != std::string::npos)
	{
		std::string my_email = file_data.substr(0, pos);
		std::string my_password = file_data.substr(pos + 1);
		pos = my_password.find('\r');
		if (pos != std::string::npos)
			my_password = my_password.substr(0, pos);
		if ((my_email == email) && (my_password == password))
			return 1;
	}
	return 0;
}
