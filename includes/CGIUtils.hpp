#ifndef CGIUTILS_HPP
# define CGIUTILS_HPP

# include "ClientRequest.hpp"
# include "CGI.hpp"

enum Types {
    PHP,
    PYTHON,
	Null
};

int CGIchecker(std::map<std::string, std::string> clMap, std::string FILE_PATH, setOfRuleHTTP rules, int fd);
void splitPath(const std::string& fullPath, std::string& path, std::string& file);
bool isPHPInstalled();
bool isPythonInstalled();
int stoi(std::string s);

#endif
