#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <map>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sstream>
#include <cstdlib>
#include "CGIRequest.hpp"
#include "CGIUtils.hpp"

class CGI {
	public:
		CGI(const std::string suffix, const std::string cmd, const CGIRequest& request, int fd);
		~CGI();

		int execute();

	private:
		CGIRequest _request;
		std::string _suffix;
		std::string _cmd;
		int _fd;

		bool createPipes(int pipe_in[2], int pipe_out[2]);
		void setEnvironmentVariables(std::string localPath, std::string fileToExecute);
   		void sendResponse(const std::string& result) const;
		int  sendPostBody(int pipe_in[2]);
		void parseResult(const std::string& data, std::map<std::string, std::string>& headers, std::string& body) const;
};

int CGIchecker(std::string clientInfo, std::string PATH_ABS, int fd);

#endif
