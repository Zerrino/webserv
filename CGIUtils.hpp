#ifndef CGIUTILS_HPP
#define CGIUTILS_HPP

#include "CGI.hpp"

enum Types {
    PHP,
    PYTHON,
	Null
};

int CGIchecker(std::string clientInfo, std::string PATH_ABS, int fd);
std::string getTypeName(Types type);
std::string getTypeName(std::string suffix);
std::string getTypeCommand(std::string suffix);
void splitPath(const std::string& fullPath, std::string& path, std::string& file);
bool isPHPInstalled();
bool isPythonInstalled();

#endif
