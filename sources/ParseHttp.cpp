/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseHttp.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 15:03:48 by marvin            #+#    #+#             */
/*   Updated: 2024/12/04 15:03:48 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ParseHttp.hpp"

ParseHttp::ParseHttp()
{
}

ParseHttp::~ParseHttp()
{
}

ParseHttp::ParseHttp(const ParseHttp& cp)
{
	*this = cp;
}

ParseHttp& ParseHttp::operator= (const ParseHttp& cp)
{
	if (this != &cp)
		*this = cp;
	return (*this);
}

int	ParseHttp::fromSTRtoINT(std::string str)
{
	std::stringstream ss(str);
	int result;
	ss >> result;
	if (ss.fail())
		throw std::invalid_argument("Error in conversion fromSTRtoINT");
	return result;
}

long long	ParseHttp::fromSTRtoLONG(std::string str)
{
	std::stringstream ss(str);
	long long result;
	ss >> result;
	if (ss.fail())
		throw std::invalid_argument("Error in conversion fromSTRtoINT");
	return result;
}

std::vector<int>	ParseHttp::getListenVec(HttpBlock fileConfig, setOfRuleHTTP &rules)
{
	std::vector<int> ret;
	for (std::vector<ServerBlock>::const_iterator i = fileConfig.servers.begin(); i != fileConfig.servers.end(); i++)
	{
		for (std::vector<LocationBlock>::const_iterator k = i->locations.begin(); k != i->locations.end(); k++)
		{
			if (k->modifier == "")
				rules.loc[i - fileConfig.servers.begin()].push_back(*k);
			else if (k->modifier == "=")
				rules.loc_eq[i - fileConfig.servers.begin()].push_back(*k);
			else if (k->modifier == "~")
				rules.loc_cgi[i - fileConfig.servers.begin()].push_back(*k);
		}
		for (std::vector<Directive>::const_iterator j = i->directives.begin(); j != i->directives.end(); j++)
			if (j->name == "listen")
			{
				ret.push_back(fromSTRtoINT(j->arguments.begin()->value));
				rules.listen[j->arguments.begin()->value] = i - fileConfig.servers.begin();
			}
	}
	return ret;
}

//struct Directive
//{
//	std::string name;
//	std::vector<DirArgument> arguments;
//};

void	ParseHttp::getDirRules(std::vector<Directive> dir,setOfRuleHTTP &rules, bool k) // k = false http/server, k = true location
{
	for (std::vector<Directive>::iterator i = dir.begin(); i != dir.end(); i++)
	{
		if (i->name == "client_max_body_size")
		{
			rules.client_max_body_size = fromSTRtoINT(i->arguments.begin()->value);
		}
		else if (i->name == "error_page")
		{
			for (std::vector<DirArgument>::iterator j = i->arguments.begin(); j != i->arguments.end() - 1; j++)
				rules.error_page[j->value] = i->arguments.back().value;
		}
		else if (i->name == "server_name")
		{
			rules.server_name = i->arguments.begin()->value;
		}
		else if (i->name == "root")
		{
			if (k)
				rules.sub_root = i->arguments.begin()->value;
			else
				rules.root = i->arguments.begin()->value;
		}
		else if (i->name == "limit_except")
		{
			rules.limit_except.clear();
			for (std::vector<DirArgument>::iterator j = i->arguments.begin(); j != i->arguments.end(); j++)
			{
				rules.limit_except.push_back(j->value);
			}
		}
		else if (i->name == "autoindex")
		{
			rules.autoindex = i->arguments.begin()->value;
		}
		else if (i->name == "fastcgi_param")
		{
			rules.fastcgi_param = i->arguments.begin()->value;
		}
		else if (i->name == "index")
		{
			rules.index.clear();
			for (std::vector<DirArgument>::iterator j = i->arguments.begin(); j != i->arguments.end(); j++)
			{
				rules.index.push_back(j->value);
			}
		}
		else if (i->name == "return")
		{
			rules.return_code = i->arguments.front().value;
			rules.return_page = i->arguments.back().value;
		}
	}
}

void	ParseHttp::printSetOfRuleHTTP(setOfRuleHTTP config)
{
	std::cout << "client_max_body_size: ";
	if (config.client_max_body_size == -1)
		std::cout << "don't exist" << std::endl;
	else
		std::cout << config.client_max_body_size << std::endl;
	std::cout << "server_name: ";
	if (config.server_name.empty())
		std::cout << "don't exist" << std::endl;
	else
		std::cout << config.server_name << std::endl;
	std::cout << "root: ";
	if (config.root.empty())
		std::cout << "don't exist" << std::endl;
	else
		std::cout << config.root << std::endl;
	std::cout << "sub_root: ";
	if (config.sub_root.empty())
		std::cout << "don't exist" << std::endl;
	else
		std::cout << config.sub_root << std::endl;
	std::cout << "autoindex: ";
	if (config.autoindex.empty())
		std::cout << "don't exist" << std::endl;
	else
		std::cout << config.autoindex << std::endl;
	std::cout << "fastcgi_param: ";
	if (config.fastcgi_param.empty())
		std::cout << "don't exist" << std::endl;
	else
		std::cout << config.fastcgi_param << std::endl;
	std::cout << "return : ";
	if (config.return_code.empty())
		std::cout << "don't exist" << std::endl;
	else
		std::cout << config.return_code << " " << config.return_page << std::endl;
	std::cout << "limit_except: ";
	if (config.limit_except.empty())
		std::cout << "don't exist" << std::endl;
	else {
		for (size_t i = 0; i < config.limit_except.size(); ++i)
			std::cout << config.limit_except[i] << (i + 1 == config.limit_except.size() ? "\n" : ", ");
	}
	std::cout << "index: ";
	if (config.index.empty())
		std::cout << "don't exist" << std::endl;
	else {
		for (size_t i = 0; i < config.index.size(); ++i)
			std::cout << config.index[i] << (i + 1 == config.index.size() ? "\n" : ", ");
	}
	std::cout << "error_page: ";
	if (config.error_page.empty())
		std::cout << "don't exist" << std::endl;
	else {
		for (std::map<std::string, std::string>::const_iterator it = config.error_page.begin(); it != config.error_page.end(); ++it) {
			std::map<std::string, std::string>::const_iterator next = it;
			++next;
			std::cout << it->first << " -> " << it->second;
			if (next != config.error_page.end())
				std::cout << ", ";
			else
				std::cout << std::endl;
		}
	}
	std::cout << "listen: ";
	if (config.listen.empty())
		std::cout << "don't exist" << std::endl;
	else {
		for (std::map<std::string, int>::const_iterator it = config.listen.begin(); it != config.listen.end(); ++it) {
			std::map<std::string, int>::const_iterator next = it;
			++next;
			std::cout << it->first << " -> " << it->second;
			if (next != config.listen.end())
				std::cout << ", ";
			else
				std::cout << std::endl;
		}
	}
}
