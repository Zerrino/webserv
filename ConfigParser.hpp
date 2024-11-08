#pragma once
#include <iostream>
#include <string>
#include "webserv.hpp"

#define DEFAULT_CONFIG_PATH "Config/default.conf"

enum TokenType
{
	TOKEN_KEYWORD_HTTP,
	TOKEN_KEYWORD_SERVER,
	TOKEN_KEYWORD_LOCATION,
	TOKEN_KEYWORD_INCLUDE,
	TOKEN_KEYWORD_LIMIT,
	TOKEN_SYMBOL_OPEN_BRACE,
	TOKEN_SYMBOL_CLOSE_BRACE,
	TOKEN_SYMBOL_SEMICOLON,
	TOKEN_IDENTIFIER,
	TOKEN_STRING,
	TOKEN_STRING_QUOTED,
	TOKEN_NUMBER,
	TOKEN_NUMBER_WITH_UNIT,
	TOKEN_REGEX,
	TOKEN_VARIABLE,
	TOKEN_OPERATOR_EQUAL,
	TOKEN_OPERATOR_NOT_EQUAL
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

// struct Location
// {
// 	std::string path;
// 	std::string root;
// 	std::vector<std::string> index_files;
// 	bool autoindex;
// 	std::string return_code;
// 	std::string return_url;
// 	size_t client_max_body_size;
// 	std::vector<std::string> allowed_methods;
//     bool deny_all;
//     std::string fastcgi_pass;
//     std::map<std::string, std::string> fastcgi_params;
// };

// struct Server
// {
// 	std::string listen;
// 	std::string server_name;
// 	std::vector<Location> locations;
// 	std::map<std::string, std::string> error_pages;
// };

// struct HttpConfig
// {
// 	size_t client_max_body_size;
// 	std::vector<std::string> error_page;
// 	std::vector<Server> servers;
// };

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
	/* Utils function for parsing : create a new class for that ??? */
	std::vector<std::string> split(std::ifstream &file);
	TokenType getTokenType(const std::string& word);
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
};

std::ostream &operator<<(std::ostream &o, ConfigParser const &i);
