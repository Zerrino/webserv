/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdelvign <gdelvign@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 16:56:26 by gdelvign          #+#    #+#             */
/*   Updated: 2024/11/08 10:29:46 by gdelvign         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"
#include "webserv.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ConfigParser::ConfigParser() : _configPath(DEFAULT_CONFIG_PATH)
{
}

ConfigParser::ConfigParser(char *path) : _configPath(path)
{
}

ConfigParser::ConfigParser(const ConfigParser &src)
{
	if (this != &src)
		*this = src;
	return;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ConfigParser::~ConfigParser()
{
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

ConfigParser &ConfigParser::operator=(ConfigParser const &rhs)
{
	if (this != &rhs)
	{
		*this = rhs;
	}
	return *this;
}

std::ostream &operator<<(std::ostream &o, ConfigParser const &i)
{
	(void)i;
	// o << "Value = " << i.getValue();
	return o;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

ConfigParser::ConfigError ConfigParser::checkPathValidity()
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

ConfigParser::ConfigError ConfigParser::open()
{
	_configFile.open(_configPath);
	if (!_configFile.is_open())
		return (NOT_OPEN);
	if (_configFile.peek() == std::ifstream::traits_type::eof())
		return (EMPTY_FILE);
	return (SUCCESS);
}

std::string ConfigParser::fetchErrorMsg(ConfigError code)
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
		return "Error: File could not be opened";
	case EMPTY_FILE:
		return "Error: File seems to be empty";
	default:
		return "Unknown error";
	}
}

ConfigParser::ConfigError ConfigParser::parse()
{
	std::vector<std::string> words = split(_configFile);

	// Print all words for testing purpose
	for (std::vector<std::string>::const_iterator i = words.begin(); i != words.end(); i++)
		std::cout << *i << std::endl;

	

	return (SUCCESS);
}

/*
** --------------------------------- UTILS ---------------------------------
*/

/*
** This function splits the whole config file in 'words' and skips the comments
*/
std::vector<std::string> ConfigParser::split(std::ifstream &file)
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
		else if (!std::isspace(ch) && ch != ';')
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

void ConfigParser::initTokenMap()
{
	_tokenMap["http"] = TOKEN_KEYWORD_HTTP;
	_tokenMap["server"] = TOKEN_KEYWORD_SERVER;
	_tokenMap["location"] = TOKEN_KEYWORD_LOCATION;
	_tokenMap["include"] = TOKEN_KEYWORD_INCLUDE;
	_tokenMap["limit_except"] = TOKEN_KEYWORD_LIMIT;
	_tokenMap["client_max_body_size"] = TOKEN_IDENTIFIER;
	_tokenMap["error_page"] = TOKEN_IDENTIFIER;
	_tokenMap["listen"] = TOKEN_IDENTIFIER;
	_tokenMap["server_name"] = TOKEN_IDENTIFIER;
	_tokenMap["root"] = TOKEN_IDENTIFIER;
	

}

TokenType ConfigParser::getTokenType(const std::string &word)
{
	if (word == "{")
		return TOKEN_SYMBOL_OPEN_BRACE;
	else if (word == "}")
		return TOKEN_SYMBOL_CLOSE_BRACE;
	else if (word == ";")
		return TOKEN_SYMBOL_SEMICOLON;
	else if (word == "=")
		return TOKEN_OPERATOR_EQUAL;
	else if (word == "!=")
		return TOKEN_OPERATOR_NOT_EQUAL;

	initTokenMap();
	std::map<std::string, TokenType>::iterator it = _tokenMap.find(word);
	if (it !=_tokenMap.end())
		return (it->second);

	
	
	// WIP
}

bool ConfigParser::expectedToken(const std::string &expected)
{
	if (_currentPosition >= _tokens.size())
		return false;
	return (_tokens[_currentPosition].value == expected);
}

void ConfigParser::getNextToken()
{
	if (_currentPosition < _tokens.size())
		_currentPosition++;
}

bool ConfigParser::expectedAndMove(const std::string &expected)
{
	if (expectedToken(expected))
	{
		getNextToken();
		return (true);
	}
	return (false);
}

bool ConfigParser::parsehttp()
{
	// WIP
}

/* ************************************************************************** */
