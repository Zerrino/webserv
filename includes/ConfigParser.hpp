/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdelvign <gdelvign@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 15:12:33 by gdelvign          #+#    #+#             */
/*   Updated: 2024/11/21 16:08:23 by gdelvign         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

#define DEFAULT_CONFIG_PATH "Config/default.conf"

enum TokenType
{
	TOKEN_BLOCK_HTTP,
	TOKEN_BLOCK_SERVER,
	TOKEN_BLOCK_LOCATION,
	TOKEN_SYMBOL_OPEN_BRACE,
	TOKEN_SYMBOL_CLOSE_BRACE,
	TOKEN_SYMBOL_SEMICOLON,
	TOKEN_OPERATOR_EQUAL,
	TOKEN_OPERATOR_NOT_EQUAL,
	TOKEN_IDENTIFIER,
	TOKEN_NUMBER,
	TOKEN_NUMBER_WITH_UNIT,
	TOKEN_VARIABLE,
	TOKEN_REGEX,
	TOKEN_STRING,
	TOKEN_STRING_QUOTED,
	INVALID_TOKEN
};

struct Token
{
	std::string value;
	TokenType type;
};

struct Directive
{
	std::string name;
	std::vector<std::string> arguments;
};

struct LocationBlock
{
	std::string match;
	std::vector<Directive> directives;
};

struct ServerBlock
{
	std::vector<Directive> directives;
	std::vector<LocationBlock> locations;
};

struct HttpBlock
{
	std::vector<ServerBlock> servers;
};

class ConfigParser
{

public:
	enum ConfigError
	{
		SUCCESS,
		NOT_EXIST,
		NOT_FILE,
		NOT_ACCESSIBLE,
		NOT_OPEN,
		EMPTY_FILE
	};

	/* Constructors */
	ConfigParser();
	ConfigParser(char *path);
	ConfigParser(ConfigParser const &src);

	/* Destructor */
	~ConfigParser();

	/* Overload */
	ConfigParser &operator=(ConfigParser const &rhs);

	/* Methods */
	ConfigError checkPathValidity();
	ConfigError open();
	ConfigError parse();
	std::string fetchErrorMsg(ConfigError code);

	/* Utils */
	std::vector<std::string> split(std::ifstream &file);
	TokenType getTokenType(const std::string &word);
	int isNumber(const std::string &word);
	bool isUnit(char c) const;
	int isOperator(const std::string &word);
	bool isVariable(const std::string &word);
	bool isRegex(const std::string &word);
	int isString(const std::string &word);
	void initializeUnits();
	void initTokenMap();
	bool expectedToken(const std::string &expected);
	void getNextToken();
	bool expectedAndMove(const std::string &expected);
	bool parsehttp();

private:
	std::string _configPath;
	std::ifstream _configFile;
	std::vector<Token> _tokens;
	size_t _currentPosition;
	std::map<std::string, TokenType> _tokenMap;
	std::vector<char> _units;
};

std::ostream &operator<<(std::ostream &o, ConfigParser const &i);
