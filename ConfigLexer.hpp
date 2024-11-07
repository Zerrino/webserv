#pragma once
#include <iostream>
#include <string>
#include "webserv.hpp"

class ConfigLexer
{

public:
	enum ConfigError
	{
		SUCCESS,
		NOT_EXIST,
		NOT_FILE,
		NOT_ACCESSIBLE,
		NOT_OPEN
	};
	/* Constructors */
	ConfigLexer();
	ConfigLexer(char *path);
	ConfigLexer(ConfigLexer const &src);
	/* Destructor */
	~ConfigLexer();
	/* Overload */
	ConfigLexer &operator=(ConfigLexer const &rhs);
	/* Methods */
	ConfigError checkPathvalidity();
	ConfigError	open();
	std::string fetchErrorMsg(ConfigError code);
	ConfigError tokenize();
	std::vector<std::string> split(std::ifstream &file);

private:
	std::string _configPath;
	std::ifstream _configFile;
	// std::vector<Token> _tokens;
};

std::ostream &operator<<(std::ostream &o, ConfigLexer const &i);
