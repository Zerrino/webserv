/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdelvign <gdelvign@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 15:12:33 by gdelvign          #+#    #+#             */
/*   Updated: 2024/11/26 22:18:06 by gdelvign         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

#define DEFAULT_CONFIG_PATH "Config/default.conf"

class ConfigParser;

/*
** List of all token types that could be found in config file
*/

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
	TOKEN_MODIFIER,
	TOKEN_DIRECTIVE,
	TOKEN_NUMBER,
	TOKEN_NUMBER_WITH_UNIT,
	TOKEN_VARIABLE,
	TOKEN_REGEX,
	TOKEN_STRING,
	TOKEN_STRING_QUOTED,
	INVALID_TOKEN,
	SENTINELLE = -1,
};

/*
** Enum useful for parsing to determine at which level
** to store the directive's values
*/

enum Context
{
	HTTP_BLOCK,
	SERVER_BLOCK,
	LOCATION_BLOCK
};

/*
** Token structure to type each collected word from config file
*/

struct Token
{
	std::string value;
	TokenType type;
};

/* AST structures to store each relevant value from location
** and directives at different context levels : http, server, location
*/

struct DirArgument
{
	std::string value;
	TokenType type;
};

struct Directive
{
	std::string name;
	std::vector<DirArgument> arguments;
};

struct LocationBlock
{
	std::string modifier;
	std::string uri;
	std::vector<Directive> directives;
};

struct ServerBlock
{
	std::vector<Directive> directives;
	std::vector<LocationBlock> locations;
};

struct HttpBlock
{
	std::vector<Directive> directives;
	std::vector<ServerBlock> servers;
};

/*
** Structure to store directives specifications
*/

struct DirectiveSpec
{
	std::string name;
	size_t maxArgs;
	TokenType argTypes[5];
	bool (ConfigParser::*validator)(const std::vector<DirArgument> &args, DirectiveSpec spec);
};


/* Parsing class */

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
		EMPTY_FILE,
		PARSING_ERROR
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
	bool isDirective(const std::string &word);
	int isNumber(const std::string &word);
	bool isUnit(char c) const;
	int isOperator(const std::string &word);
	bool isVariable(const std::string &word);
	bool isRegex(const std::string &word);
	int isString(const std::string &word);
	void initializeUnits();
	void initTokenMap();
	bool expectedTokenType(TokenType expectedType);
	Token getNextToken();
	bool expectedAndMove(TokenType expectedType);
	bool parseHttp();
	bool parseServer();
	bool parseLocation();
	bool parseDirective(Context context);
	bool isValidArgType(const Token &token);
	bool reportSyntaxError(const std::string &error);
	void validateDirectives();
	bool CheckClientMaxBodySize(const std::vector<DirArgument> &args, DirectiveSpec spec);

	/* TESTING PURPOSE */
	void printConfig();

private:
	std::string _configPath;
	std::ifstream _configFile;
	std::vector<Token> _tokens;
	size_t _currentPosition;
	std::map<std::string, TokenType> _tokenMap;
	std::vector<char> _units;
	HttpBlock _http;
	int _server_id;
	int _loc_id;
};

std::ostream &operator<<(std::ostream &o, ConfigParser const &i);

/*
** Static list of directives to store all of them
** with their specifications
*/

static const DirectiveSpec directives[] = {
	{"client_max_body_size",
	 1,
	 {TOKEN_NUMBER_WITH_UNIT, SENTINELLE},
	 &ConfigParser::CheckClientMaxBodySize,
	},
	{"error_page",
	 1,
	 {TOKEN_NUMBER, TOKEN_STRING, SENTINELLE},
	 nullptr},
	{"listen",
	 2,
	 {TOKEN_NUMBER, SENTINELLE},
	 nullptr},
	{"server_name",
	 5,
	 {TOKEN_STRING, SENTINELLE},
	 nullptr},
	{"root",
	 1,
	 {TOKEN_STRING, SENTINELLE},
	 nullptr},
	{"limit_except",
	 5,
	 {TOKEN_STRING, SENTINELLE},
	 nullptr},
	{"autoindex",
	 1,
	 {TOKEN_STRING, SENTINELLE},
	 nullptr},
	{"client_body_temp_path",
	 1,
	 {TOKEN_STRING, SENTINELLE},
	 nullptr},
	{"client_body_in_file_only",
	 1,
	 {TOKEN_STRING, SENTINELLE},
	 nullptr},
	{"fastcgi_pass",
	 1,
	 {TOKEN_STRING, SENTINELLE},
	 nullptr},
	{"fastcgi_param",
	 1,
	 {TOKEN_STRING, TOKEN_VARIABLE, SENTINELLE},
	 nullptr},
	{"index",
	 5,
	 {TOKEN_STRING, SENTINELLE},
	 nullptr},
	{"return",
	 2,
	 {TOKEN_NUMBER, TOKEN_STRING, SENTINELLE},
	 nullptr},
	{"include",
	 1,
	 {TOKEN_STRING, SENTINELLE},
	 nullptr},
};
