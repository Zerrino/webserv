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

	for (std::vector<std::string>::const_iterator i = words.begin(); i != words.end(); i++)
	{
		Token token;

		token.value = *i;
		token.type = getTokenType(*i);
		if (token.type == INVALID_TOKEN)
		{
			std::cerr << "Syntax error : invalid token at '" << token.value << "'" << std::endl;
			return (PARSING_ERROR);
		}
		_tokens.push_back(token);
	}
	if (parseHttp())
		return (PARSING_ERROR);
	return (SUCCESS);
}

bool ConfigParser::parseHttp()
{
	_server_id = 0;
	_loc_id = 0;

	if (!expectedAndMove(TOKEN_BLOCK_HTTP))
		return reportSyntaxError("Config file should begin by a 'http' block");
	if (!expectedAndMove(TOKEN_SYMBOL_OPEN_BRACE))
		return reportSyntaxError("Expected '{' after 'http'");
	while (!expectedTokenType(TOKEN_SYMBOL_CLOSE_BRACE))
	{
		if (expectedTokenType(TOKEN_IDENTIFIER))
		{
			if (!parseDirective(HTTP_BLOCK, 0, 0))
				return (false);
		}
		else if (expectedTokenType(TOKEN_BLOCK_SERVER))
		{
			if (!parseServer())
				return (false);
		}
		else
			return reportSyntaxError("Unexpected token in 'http' : '" + _tokens[_currentPosition].value + "'");
	}
	if (!expectedAndMove(TOKEN_SYMBOL_CLOSE_BRACE))
		return reportSyntaxError("Missing closing bracket in 'http' block");
	std::cout << "ok" << std::endl;
	return (true);
}

bool ConfigParser::parseServer()
{
	if (!expectedAndMove(TOKEN_BLOCK_SERVER))
		return reportSyntaxError("Server block expected instead of '" + _tokens[_currentPosition].value + "'");
	if (!expectedAndMove(TOKEN_SYMBOL_OPEN_BRACE))
		return reportSyntaxError("Expected '{' after 'server'");
	while (!expectedTokenType(TOKEN_SYMBOL_CLOSE_BRACE))
	{
		if (expectedTokenType(TOKEN_IDENTIFIER))
		{
			if (!parseDirective(SERVER_BLOCK, _server_id, _loc_id))
				return (false);
		}
		else if (expectedTokenType(TOKEN_BLOCK_LOCATION))
		{
			if (!parseLocation())
				return (false);
		}
		else
			return reportSyntaxError("Unexpected token in 'server' : '" + _tokens[_currentPosition].value + "'");
	}
	_server_id++;
	return (true);
} 

bool ConfigParser::parseLocation()
{
	_loc_id++;
	return (true);
}

bool ConfigParser::parseDirective(Context context, int server_id, int loc_id)
{
	Directive temp;

	if (!expectedTokenType(TOKEN_IDENTIFIER))
		return reportSyntaxError("Directive name expected instead of '" + _tokens[_currentPosition].value + "'");
	temp.name = _tokens[_currentPosition].value;

	while (!expectedTokenType(TOKEN_SYMBOL_SEMICOLON))
	{
		if (_currentPosition >= _tokens.size())
			return reportSyntaxError("Unexpected end of input in directive arguments");
		temp.arguments.push_back(_tokens[_currentPosition].value);
		_currentPosition++;
	}

	if (context == HTTP_BLOCK)
		_http.directives.push_back(temp);
	else if (context == SERVER_BLOCK)
	{
		_http.servers[server_id].directives.push_back(temp);
	}
	else
		_http.servers[server_id].locations[loc_id].directives.push_back(temp);
	_currentPosition++;
	return (true);
}

bool ConfigParser::reportSyntaxError(const std::string &error)
{
	std::cout << "Syntax error : " << error << std::endl;
	return (false);
}

bool ConfigParser::expectedTokenType(TokenType expectedType)
{
	if (_currentPosition >= _tokens.size())
		return false;
	return (_tokens[_currentPosition].type == expectedType);
}

void ConfigParser::getNextToken()
{
	if (_currentPosition < _tokens.size())
		_currentPosition++;
}

bool ConfigParser::expectedAndMove(TokenType expectedType)
{
	if (expectedTokenType(expectedType))
	{
		getNextToken();
		return (true);
	}
	return (false);
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
		if (data[i] == '#')
		{
			while (i < data.length() && data[i] != '\n')
				i++;
			i++;
		}
		else if (data[i] == '~' || data[i] == '^')
		{
			start = i;
			i++;
			data[i] == '*' ? i += 2 : i++;
			while (data[i] != ' ')
				i++;
			word = data.substr(start, i - start);
			words.push_back(word);
			word.clear();
		}
		else if (data[i] == '$')
		{
			start = i;
			i++;
			while (i < data.length() && (isalnum(data[i]) || data[i] == '_'))
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
	return (words);
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
	_tokenMap["deny"] = TOKEN_IDENTIFIER;
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
			if ((j == (word.length() - 1) && isUnit(*word.rbegin())) || (j == (word.length() - 2) && (*word.rbegin() == 's') && (*word.rbegin() == 'm')))
				return (TOKEN_NUMBER_WITH_UNIT);
		}
		return (INVALID_TOKEN);
	}
	return (0);
}

int ConfigParser::isOperator(const std::string &word)
{
	if (word == "=")
		return (TOKEN_OPERATOR_EQUAL);
	else if (word == "!=")
		return (TOKEN_OPERATOR_NOT_EQUAL);
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

bool ConfigParser::isRegex(const std::string &word)
{
	if (word[0] == '~' || word[0] == '^')
		return (true);
	return (false);
}

int ConfigParser::isString(const std::string &word)
{
	if (word[0] == '"' || word == "'")
	{
		char quote_type = word[0];
		if (word.find(quote_type, word.length()) != std::string::npos)
			return (TOKEN_STRING_QUOTED);
		else
			return (INVALID_TOKEN);
	}
	else
		return (TOKEN_STRING);
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
	int ret;
	std::map<std::string, TokenType>::iterator it = _tokenMap.find(word);
	if (it != _tokenMap.end())
		return (it->second);
	else if ((ret = isNumber(word)))
		return ((TokenType)ret);
	else if ((ret = isOperator(word)))
		return ((TokenType)ret);
	else if (isVariable(word))
		return (TOKEN_VARIABLE);
	else if (isRegex(word))
		return (TOKEN_REGEX);
	else if ((ret = isString(word)))
		return ((TokenType)ret);
	return (INVALID_TOKEN);
}

/* ************************************************************************** */
