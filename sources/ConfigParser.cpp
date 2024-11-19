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

#include "../includes/ConfigParser.hpp"
#include "../includes/webserv.hpp"

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
	_configFile.open(_configPath.c_str());
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
	{
		Token token;

		token.value = *i;
		token.type = getTokenType(*i);
		_tokens.push_back(token);
		std::cout << token.value << std::endl;
		// if (token.value[0] == '$')
		//	std::cout << token.value << std::endl << " Type = " << token.type << std::endl;
	}

	return (SUCCESS);
}

/*
** --------------------------------- UTILS ---------------------------------
*/

/*
** This function splits the whole config file in 'words/tokens' and skips the comments
*/
std::vector<std::string> ConfigParser::split(std::ifstream &file)
{
	std::vector<std::string> words;
	std::string word;
	size_t start = 0;

	std::string data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	size_t i = 0;
	while (i < data.length())
	{
		// std::cout << data[i] << std::endl;
		// std::cout << i << std::endl;
		if (data[i] == '#')
		{
			while (i < data.length() && data[i] != '\n')
				i++;
			i++;
		}
		else if (data[i] == '$')
		{
			start = i;
			while (i < data.length() && (isalnum(data[i]) || data[i] == '_' || data[i] == '$'))
				i++;
			word = data.substr(start, i - start);
			words.push_back(word);
			word.clear();
		}
		else if (data[i] == ';')
		{
			word = data[i];
			words.push_back(word);
			word.clear();
			i++;
		}

		else if (!std::isspace(data[i]))
		{
			start = i;
			while (i < data.length() && !std::isspace(data[i]) && data[i] != ';' && data[i] != '$' && data[i] != '#')
				i++;
			word = data.substr(start, i - start);
			words.push_back(word);
			word.clear();
		}
		else
			i++;
	}
	std::cout << "i = " << i << std::endl;
	std::cout << "data =" << data.length() << std::endl;
	return (words);

	// while (file.get(ch))
	// {
	// 	if (ch == '#')
	// 	{
	// 		while (ch != '\n')
	// 			file.get(ch);
	// 		ch++;
	// 	}
	// 	else if (ch == '$')
	// 	{
	// 		while (isalnum(ch) || ch == '_')
	// 		{
	// 			word += ch;
	// 			file.get(ch);
	// 		}
	// 		words.push_back(word);
	// 		word.clear();
	// 		// std::cout << word << std::endl;
	// 		if (ch == '$')
	// 			continue ;
	// 	}
	// 	else if (!std::isspace(ch) && ch != ';' && ch != '$')
	// 		word += ch;
	// 	else
	// 	{
	// 		if (!word.empty())
	// 		{
	// 			words.push_back(word);
	// 			word.clear();
	// 		}
	// 		if (ch == ';')
	// 		{
	// 			words.push_back(";");
	// 			word.clear();
	// 		}
	// 	}
	// }
	// if (!word.empty())
	// 	words.push_back(word);
	// return (words);
}

void ConfigParser::initTokenMap()
{
	/* Blocks */
	_tokenMap["http"] = TOKEN_BLOCK_HTTP;
	_tokenMap["server"] = TOKEN_BLOCK_SERVER;
	_tokenMap["location"] = TOKEN_BLOCK_LOCATION;

	/* Symbols */
	_tokenMap["{"] = TOKEN_SYMBOL_OPEN_BRACE;
	_tokenMap["}"] = TOKEN_SYMBOL_CLOSE_BRACE;
	_tokenMap[";"] = TOKEN_SYMBOL_SEMICOLON;
	_tokenMap["="] = TOKEN_OPERATOR_EQUAL;
	_tokenMap["!="] = TOKEN_OPERATOR_NOT_EQUAL;

	/* Identifiers */
	_tokenMap["client_max_body_size"] = TOKEN_IDENTIFIER;
	_tokenMap["error_page"] = TOKEN_IDENTIFIER;
	_tokenMap["listen"] = TOKEN_IDENTIFIER;
	_tokenMap["server_name"] = TOKEN_IDENTIFIER;
	_tokenMap["root"] = TOKEN_IDENTIFIER;
	_tokenMap["limit_except"] = TOKEN_IDENTIFIER;
	_tokenMap["autoindex"] = TOKEN_IDENTIFIER;
	_tokenMap["client_body_temp_path"] = TOKEN_IDENTIFIER;
	_tokenMap["client_body_in_file_only"] = TOKEN_IDENTIFIER;
	_tokenMap["client_max_body_size"] = TOKEN_IDENTIFIER;
	_tokenMap["fastcgi_pass"] = TOKEN_IDENTIFIER;
	_tokenMap["fastcgi_params"] = TOKEN_IDENTIFIER;
	_tokenMap["include"] = TOKEN_IDENTIFIER;
	_tokenMap["return"] = TOKEN_IDENTIFIER;
}

int ConfigParser::isNumber(const std::string &word)
{
	size_t i = 0;
	size_t j = 0;

	initializeUnits();
	if (isdigit(word[i]))
	{
		for (i = 0; i < word.length(); i++)
		{
			if (isdigit(word[i]))
				j++;
			else
				break;
		}
		if (j == word.length())
			return (TOKEN_NUMBER);
		if (j == (word.length() - 1) || j == (word.length() - 2))
		{
			if ((j == (word.length() - 1) && isUnit(word[word.length()])) || (j == (word.length() - 2) && ((word[word.length()] == 's' && (word[word.length()] - 1) == 'm'))))
				return (TOKEN_NUMBER_WITH_UNIT);
		}
	}
	return (0);
}

bool ConfigParser::isVariable(const std::string &word)
{
	if (word[0] == '$')
	{
		for (size_t i = 1; i < word.length(); i++)
		{
			if (!(isalnum(word[i]) || word[i] == '_'))
				return (false);
		}
		return (true);
	}
	return (false);
}

int ConfigParser::isOperator(const std::string &word)
{
	if (word == "=")
		return (TOKEN_OPERATOR_EQUAL);
	else if (word == "!=")
		return (TOKEN_OPERATOR_NOT_EQUAL);
	return (0);
}

void ConfigParser::initializeUnits()
{
	_units.push_back('k');
	_units.push_back('K');
	_units.push_back('m');
	_units.push_back('M');
	_units.push_back('g');
	_units.push_back('G');
	_units.push_back('s');
	_units.push_back('h');
	_units.push_back('d');
	_units.push_back('w');
	_units.push_back('M');
	_units.push_back('y');
}

bool ConfigParser::isUnit(char c) const
{
	return std::find(_units.begin(), _units.end(), c) != _units.end();
}

TokenType ConfigParser::getTokenType(const std::string &word)
{
	initTokenMap();
	std::map<std::string, TokenType>::iterator it = _tokenMap.find(word);
	if (it != _tokenMap.end())
		return (it->second);
	else if (int ret = isNumber(word))
		return ((TokenType)ret);
	else if (int ret = isOperator(word))
		return ((TokenType)ret);
	else if (isVariable(word))
		return (TOKEN_VARIABLE);

	return (INVALID_TOKEN);
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
	return (true);
}

/* ************************************************************************** */
