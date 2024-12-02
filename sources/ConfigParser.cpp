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
	case PARSING_ERROR:
		return "Error: Configuration file could not be parsed due to a syntax error";
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
			std::cerr << "Syntax error: invalid token at '" << token.value << "'" << std::endl;
			return (PARSING_ERROR);
		}
		_tokens.push_back(token);
	}
	if (!parseHttp())
		return (PARSING_ERROR);

	// TESTING
	printConfig();

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
		if (_currentPosition >= _tokens.size())
			return reportSyntaxError("Unexpected end of file. Missing closing bracket '}' in 'http' block.");
		if (expectedTokenType(TOKEN_DIRECTIVE))
		{
			if (!parseDirective(HTTP_BLOCK))
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
	return (true);
}

bool ConfigParser::parseServer()
{
	_loc_id = 0;
	ServerBlock server;
	_http.servers.push_back(server);

	if (!expectedAndMove(TOKEN_BLOCK_SERVER))
		return reportSyntaxError("Server block expected instead of '" + _tokens[_currentPosition].value + "'");
	if (!expectedAndMove(TOKEN_SYMBOL_OPEN_BRACE))
		return reportSyntaxError("Expected '{' after 'server'");
	while (!expectedTokenType(TOKEN_SYMBOL_CLOSE_BRACE))
	{
		if (_currentPosition >= _tokens.size())
			return reportSyntaxError("Unexpected end of file. Missing closing bracket '}' in 'server' block.");
		if (expectedTokenType(TOKEN_DIRECTIVE))
		{
			if (!parseDirective(SERVER_BLOCK))
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
	if (!expectedAndMove(TOKEN_SYMBOL_CLOSE_BRACE))
		return reportSyntaxError("Missing closing bracket in 'server' block");
	_server_id++;
	return (true);
}

bool ConfigParser::parseLocation()
{
	LocationBlock temp;

	if (!expectedTokenType(TOKEN_BLOCK_LOCATION))
		return reportSyntaxError("Location expected instead of '" + _tokens[_currentPosition].value + "'");
	getNextToken();
	if (expectedTokenType(TOKEN_OPERATOR_EQUAL) || expectedTokenType(TOKEN_MODIFIER))
	{
		temp.modifier = _tokens[_currentPosition].value;
		getNextToken();
	}
	if (expectedTokenType(TOKEN_STRING) || expectedTokenType(TOKEN_REGEX))
		temp.uri = _tokens[_currentPosition].value;
	else
		return reportSyntaxError("Location URI is missing or invalid at '" + _tokens[_currentPosition].value + "'");
	getNextToken();
	if (!expectedAndMove(TOKEN_SYMBOL_OPEN_BRACE))
		return reportSyntaxError("Expected '{' after 'location'");
	_http.servers[_server_id].locations.push_back(temp);
	while (!expectedTokenType(TOKEN_SYMBOL_CLOSE_BRACE))
	{
		if (!parseDirective(LOCATION_BLOCK))
			return (false);
	}
	if (!expectedAndMove(TOKEN_SYMBOL_CLOSE_BRACE))
		return reportSyntaxError("Missing closing bracket in 'location' block");
	_loc_id++;
	return (true);
}

bool ConfigParser::parseDirective(Context context)
{
	Directive temp;
	DirArgument tempArg;

	if (!expectedTokenType(TOKEN_DIRECTIVE))
		return reportSyntaxError("Directive name expected instead of '" + _tokens[_currentPosition].value + "'");
	temp.name = _tokens[_currentPosition].value;
	if (!isValidArgType(getNextToken()))
		return reportSyntaxError("Invalid type of directive argument for " + temp.name + " at '" + _tokens[_currentPosition].value + "'");
	while (!expectedTokenType(TOKEN_SYMBOL_SEMICOLON))
	{
		if (_currentPosition >= _tokens.size())
			return reportSyntaxError("Unexpected end of input in directive arguments");
		if (!isValidArgType(_tokens[_currentPosition]))
			return reportSyntaxError("Invalid type of directive argument or semicolon missing for " + temp.name + " at '" + _tokens[_currentPosition].value + "'");
		tempArg.value = _tokens[_currentPosition].value;
		tempArg.type = _tokens[_currentPosition].type;
		temp.arguments.push_back(tempArg);
		getNextToken();
	}
	if (!validateDirective(temp))
		return (false);
	if (context == HTTP_BLOCK)
		_http.directives.push_back(temp);
	else if (context == SERVER_BLOCK)
		_http.servers[_server_id].directives.push_back(temp);
	else
		_http.servers[_server_id].locations[_loc_id].directives.push_back(temp);
	if (!expectedAndMove(TOKEN_SYMBOL_SEMICOLON))
		return reportSyntaxError("Missing semicolon at the end of the directive '" + temp.name + "'");
	return (true);
}

bool ConfigParser::validateDirective(Directive &directive)
{
	int arr_size = sizeof(directives) / sizeof(DirectiveSpec);
	DirectiveSpec current;
	current.name = "";

	for (int i = 0; i < arr_size; i++)
	{
		if (directive.name == directives[i].name)
		{
			current = directives[i];
			break;
		}
	}
	if (current.validator)
	{
		if (!(this->*current.validator)(directive.arguments, current))
			return (false);
	}
	return (true);
}

bool ConfigParser::reportSyntaxError(const std::string &error)
{
	std::cout << "Syntax error: " << error << std::endl;
	return (false);
}

bool ConfigParser::expectedTokenType(TokenType expectedType)
{
	if (_currentPosition >= _tokens.size())
		return false;
	return (_tokens[_currentPosition].type == expectedType);
}

Token ConfigParser::getNextToken()
{
	if (_currentPosition < _tokens.size())
		_currentPosition++;
	return (_tokens[_currentPosition]);
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

bool ConfigParser::isValidArgType(const Token &token)
{
	if (token.type == TOKEN_STRING || token.type == TOKEN_STRING_QUOTED || token.type == TOKEN_NUMBER || token.type == TOKEN_NUMBER_WITH_UNIT || token.type == TOKEN_VARIABLE)
		return (true);
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
			while (data[i] != ' ')
				i++;
			word = data.substr(start, i - start);
			words.push_back(word);
			word.clear();
			i++;
			start = i;
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

	/* Modifiers */
	_tokenMap["~"] = TOKEN_MODIFIER;
	_tokenMap["~*"] = TOKEN_MODIFIER;
	_tokenMap["^~"] = TOKEN_MODIFIER;
}

bool ConfigParser::isDirective(const std::string &word)
{
	for (const DirectiveSpec *it = directives; !it->name.empty(); ++it)
	{
		if (it->name == word)
			return true;
	}
	return false;
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
			if (j == (word.length() - 1) && isUnit(*word.rbegin()))
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
	/* SIZES */
	_units.push_back('k');
	_units.push_back('K');
	_units.push_back('m');
	_units.push_back('M');
	_units.push_back('g');
	_units.push_back('G');
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
	else if (isDirective(word))
		return (TOKEN_DIRECTIVE);
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

bool ConfigParser::checkStandardDirective(std::vector<DirArgument> &args, DirectiveSpec specs)
{
	if (args.size() < 1 || args.size() > specs.maxArgs)
		return (reportSyntaxError(
			"Invalid number of arguments for " + specs.name + " directive"));
	for (std::vector<DirArgument>::const_iterator it = args.begin(); it != args.end(); it++)
	{
		int i = 0;
		int count = 0;
		while (specs.argTypes[i] != SENTINELLE)
		{
			if (specs.argTypes[i] == it->type)
				count++;
			i++;
		}
		if (!count)
			return (reportSyntaxError(
				"Wrong type of argument: " + it->value + " for " + specs.name + " directive"));
	}
	return (true);
}

bool ConfigParser::checkClientMaxBodySize(std::vector<DirArgument> &args, DirectiveSpec specs)
{
	if (args.size() < 1 || args.size() > specs.maxArgs)
		return (reportSyntaxError(
			"Invalid number of arguments for " + specs.name + " directive"));
	if (args[0].type != specs.argTypes[0])
		return (reportSyntaxError(
			"Wrong type of argument: " + args[0].value + " for " + specs.name + " directive"));
	return (true);
}

bool ConfigParser::checkListen(std::vector<DirArgument> &args, DirectiveSpec specs)
{
	int nb;
	std::istringstream(args[0].value) >> nb;

	if (args.size() < 1 || args.size() > specs.maxArgs)
		return (reportSyntaxError("Invalid number of arguments for " + specs.name + " directive"));
	if (args[0].type != TOKEN_NUMBER && (nb < 0 || nb > 65535))
		return (reportSyntaxError(
			"Wrong type of argument: " + args[0].value +
			". Please provide a port number between 0 and 65535 for " + specs.name + " directive"));
	if (args.size() > 1 && args[1].type != TOKEN_STRING)
		return (reportSyntaxError(
			"Wrong type of argument: " + args[1].value + " for " + specs.name + " directive"));
	return (true);
}

bool ConfigParser::checkLimitExcept(std::vector<DirArgument> &args, DirectiveSpec specs)
{
	static const std::string arr[] = {"GET", "POST", "PUT", "DELETE"};
	std::vector<std::string> requests(arr, arr + sizeof(arr) / sizeof(arr[0]));

	if (!checkStandardDirective(args, specs))
		return (false);
	int count = 0;
	for (std::vector<DirArgument>::const_iterator it = args.begin(); it != args.end(); it++)
	{
		count = std::count(requests.begin(), requests.end(), it->value);
		if (!count)
			return (reportSyntaxError(
				"Wrong argument: " + it->value + " for " + specs.name +
				" directive. It only accepts http requests."));
	}
	return (true);
}

bool ConfigParser::checkBoolDirective(std::vector<DirArgument> &args, DirectiveSpec specs)
{
	if (args.size() < 1 || args.size() > specs.maxArgs)
		return (reportSyntaxError(
			"Invalid number of arguments for " + specs.name + " directive"));
	if (args[0].type != TOKEN_STRING || (args[0].value != "on" && args[0].value != "off"))
		return (reportSyntaxError(
			"Wrong type of argument: " + args[0].value + " for " + specs.name +
			" directive. 'on' and 'off' only accepted."));
	return (true);
}

bool ConfigParser::checkFastCgiParam(std::vector<DirArgument> &args, DirectiveSpec specs)
{
	if (!checkStandardDirective(args, specs))
		return (false);
	if (!checkCgiPath(args, specs))
		return (false);
	return (true);
}

bool ConfigParser::checkCgiPath(std::vector<DirArgument> &args, DirectiveSpec specs)
{
	std::string path = args[0].value;

	if (path[0] == '/')
		return fileExistsAndAccessible(path, specs);
	else if (path == "$document_root")
	{
		if (args.size() < 2 || args[1].value.empty())
			return reportSyntaxError(
				"Invalid path (" + path + ") for the " + specs.name +
				" directive. The file path must be provided.");
		std::string PWD = getEnvVariable("PWD");
		std::string fullPath = PWD + args[1].value;
		if (!fileExistsAndAccessible(fullPath, specs))
			return (false);
		expandArg(args, fullPath);
	}
	else
	{
		std::string envPaths = getEnvVariable("PATH");
		std::vector<std::string> vecPaths = splitPaths(envPaths);

		bool pathOk = false;
		for (std::vector<std::string>::iterator it = vecPaths.begin(); it != vecPaths.end(); ++it)
		{
			std::string fullPath = *it + "/" + path;
			std::cout << fullPath << std::endl;
			if (access(fullPath.c_str(), F_OK | R_OK | X_OK) == 0)
			{
				pathOk = true;
				expandArg(args, fullPath);
				break;
			}
		}
		if (!pathOk)
			return reportSyntaxError(
				"Invalid path (" + path + ") for " + specs.name +
				" directive. The file doesn't exist or its access is forbidden.");
	}
	return true;
}

template <typename T>
void ConfigParser::expandArg(std::vector<DirArgument> &args, T newVal)
{
	DirArgument temp;

	args.clear();
	temp.value = newVal;
	temp.type = TOKEN_STRING;
	args.push_back(temp);
}

bool ConfigParser::fileExistsAndAccessible(const std::string &filepath, const DirectiveSpec &specs)
{
	struct stat statinfo;

	if (stat(filepath.c_str(), &statinfo) || !(S_ISREG(statinfo.st_mode)))
		return reportSyntaxError(
			"Invalid path (" + filepath + ") for " + specs.name +
			" directive. The file doesn't exist or is a directory.");
	if (access(filepath.c_str(), F_OK | R_OK | X_OK))
		return reportSyntaxError(
			"Invalid path (" + filepath + ") for the " + specs.name +
			" directive. The file must be accessible with both read and execute permissions.");
	return true;
}

std::string ConfigParser::getEnvVariable(const char *varName)
{
	const char *value = getenv(varName);
	if (!value)
		throw std::runtime_error(std::string("Unable to reach environment variable ") + varName + ".");
	return std::string(value);
}

std::vector<std::string> ConfigParser::splitPaths(const std::string &pathsStr)
{
	std::vector<std::string> vecPaths;
	size_t start = 0;
	size_t end;

	while ((end = pathsStr.find(':', start)) != std::string::npos)
	{
		vecPaths.push_back(pathsStr.substr(start, end - start));
		start = end + 1;
	}
	vecPaths.push_back(pathsStr.substr(start));
	return vecPaths;
}

bool ConfigParser::checkReturn(std::vector<DirArgument> &args, DirectiveSpec specs)
{
	if (args.size() < 1 || args.size() > specs.maxArgs)
		return (reportSyntaxError(
			"Invalid number of arguments for " + specs.name + " directive"));
	if (args[0].type != TOKEN_NUMBER)
		return (reportSyntaxError(
			"Wrong type of argument: " + args[0].value + " for " + specs.name +
			" directive. Code (number) expected"));
	if (args[1].type != TOKEN_STRING)
		return (reportSyntaxError(
			"Wrong type of argument: " + args[1].value + " for " + specs.name +
			" directive. URL (string) expected"));
	return (true);
}

bool ConfigParser::checkRoot(std::vector<DirArgument> &args, DirectiveSpec specs)
{
	if (args.size() == 1)
	{
		if (args[0].type != TOKEN_STRING)
			return reportSyntaxError("Invalid argument type for " + specs.name + " directive. Expected a string.");
	}
	else if (args.size() == 2)
	{
		if (args[0].type != TOKEN_VARIABLE || args[0].value != "$document_root")
			return reportSyntaxError("Root directive expects '$document_root' variable as the first argument.");
		if (args[1].type != TOKEN_STRING)
			return reportSyntaxError("Invalid second argument for " + specs.name + " directive. Expected a string.");
		std::string PWD = getEnvVariable("PWD");
		std::string fullPath = PWD + args[1].value;
		expandArg(args, fullPath);
	}
	else
		return reportSyntaxError("Invalid number of arguments for " + specs.name + " directive.");
	return true;
}

/* ************************************************************************** */

void ConfigParser::printConfig()
{
	// Print HTTP directives
	std::cout << "--- HTTP DIRECTIVES ---" << std::endl;
	std::cout << std::endl;
	for (std::vector<Directive>::const_iterator i = _http.directives.begin(); i != _http.directives.end(); i++)
	{
		std::cout << "Directive's name : " << i->name << std::endl;
		std::cout << i->name << "'s aguments : " << std::endl;
		for (std::vector<DirArgument>::const_iterator j = i->arguments.begin(); j != i->arguments.end(); j++)
			std::cout << j->value << std::endl;
		std::cout << std::endl;
	}
	std::cout << "--- END OF HTTP DIRECTIVES ---" << std::endl;
	std::cout << std::endl;
	// Print server blocks
	std::cout << "--- HTTP SERVERS ---" << std::endl;
	std::cout << std::endl;
	for (std::vector<ServerBlock>::const_iterator i = _http.servers.begin(); i != _http.servers.end(); i++)
	{
		// Print server directives
		std::cout << "--- SERVER DIRECTIVES ---" << std::endl;
		for (std::vector<Directive>::const_iterator j = i->directives.begin(); j != i->directives.end(); j++)
		{
			std::cout << "Directive's name : " << j->name << std::endl;
			std::cout << j->name << "'s aguments : " << std::endl;
			for (std::vector<DirArgument>::const_iterator k = j->arguments.begin(); k != j->arguments.end(); k++)
				std::cout << k->value << std::endl;
			std::cout << std::endl;
		}
		std::cout << "--- END OF SERVER DIRECTIVES ---" << std::endl;
		std::cout << std::endl;

		// Print location
		std::cout << "--- SERVER LOCATIONS ---" << std::endl;
		for (std::vector<LocationBlock>::const_iterator j = i->locations.begin(); j != i->locations.end(); j++)
		{
			std::cout << "--- LOCATION INFOS ---" << std::endl;
			std::cout << "Location modifier : " << j->modifier << std::endl;
			std::cout << "Location URI : " << j->uri << std::endl;
			std::cout << "--- LOCATION DIRECTIVES ---" << std::endl;
			for (std::vector<Directive>::const_iterator k = j->directives.begin(); k != j->directives.end(); k++)
			{
				std::cout << "Directive's name : " << k->name << std::endl;
				std::cout << k->name << "'s aguments : " << std::endl;
				for (std::vector<DirArgument>::const_iterator l = k->arguments.begin(); l != k->arguments.end(); l++)
					std::cout << l->value << std::endl;
				std::cout << std::endl;
			}
		}
		std::cout << "--- END OF OF SERVER LOCATIONS ---" << std::endl;
		std::cout << std::endl;
		std::cout << "--- END OF HTTP SERVER ---" << std::endl;
		std::cout << std::endl;
	}
}