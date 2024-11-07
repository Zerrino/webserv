/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLexer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdelvign <gdelvign@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 16:56:26 by gdelvign          #+#    #+#             */
/*   Updated: 2024/11/07 23:07:35 by gdelvign         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigLexer.hpp"
#include "webserv.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ConfigLexer::ConfigLexer() : _configPath(DEFAULT_CONFIG_PATH)
{
}

ConfigLexer::ConfigLexer(char *path) : _configPath(path)
{
}

ConfigLexer::ConfigLexer(const ConfigLexer &src)
{
	if (this != &src)
		*this = src;
	return;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ConfigLexer::~ConfigLexer()
{
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

ConfigLexer &ConfigLexer::operator=(ConfigLexer const &rhs)
{
	if (this != &rhs)
	{
		*this = rhs;
	}
	return *this;
}

std::ostream &operator<<(std::ostream &o, ConfigLexer const &i)
{
	(void)i;
	// o << "Value = " << i.getValue();
	return o;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

ConfigLexer::ConfigError ConfigLexer::checkPathvalidity()
{
	struct stat fileInfo;

	if (access(_configPath.c_str(), F_OK) < 0)
		return (NOT_EXIST);
	if (stat(_configPath.c_str(), &fileInfo) == 0)
	{
		if (S_ISDIR(fileInfo.st_mode))
			return (NOT_FILE);
		if (!(fileInfo.st_mode & S_IRUSR))
			return (NOT_ACCESSIBLE);
	}
	return (SUCCESS);
}

ConfigLexer::ConfigError ConfigLexer::open()
{
	_configFile.open(_configPath);
	if (!_configFile.is_open())
		return (NOT_OPEN);
	return (SUCCESS);
}

std::string ConfigLexer::fetchErrorMsg(ConfigError code)
{
	switch (code)
	{
	case NOT_EXIST:
		return "Error: Configuration file does not exist";
	case NOT_FILE:
		return "Error: Path is not a regular file";
	case NOT_ACCESSIBLE:
		return "Error: Configuration file is not readable";
	case NOT_OPEN:
		return "Error: The file could not be opened";
	default:
		return "Unknown error";
	}
}

/*
** This function splits the whole config file in 'words' and skips the comments
*/
std::vector<std::string> ConfigLexer::split(std::ifstream &file)
{
	std::vector<std::string> words;
	std::string word;
	char ch;

	while (file.get(ch))
	{
		if (ch == '#')
		{
			while (ch != '\n')
				file.get(ch);
			ch++;
		}
		else if (!std::isspace(ch))
			word += ch;
		else
		{
			if (!word.empty())
			{
				words.push_back(word);
				word.clear();
			}
		}
	}
	if (!word.empty())
		words.push_back(word);
	return (words);
}

ConfigLexer::ConfigError  ConfigLexer::tokenize()
{
	std::vector<std::string> words;

	words = split(_configFile);

	// Print all words for testing purpose
	for (std::vector<std::string>::const_iterator i = words.begin(); i != words.end(); i++)
		std::cout << *i << std::endl;

	// Tokenization logic here

	return (SUCCESS);
}

/*
** --------------------------------- ACCESSORS ---------------------------------
*/

/* ************************************************************************** */
