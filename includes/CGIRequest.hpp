#ifndef CGIREQUEST_HPP
#define CGIREQUEST_HPP

#include <string>
#include <map>
#include <sstream>
#include <unistd.h>
#include <iostream>
#include <map>
# include "webserv.hpp"

class CGIRequest {
	public:
		CGIRequest();
		CGIRequest(const std::map<std::string, std::string> &clMap);
		~CGIRequest();

		void send(int fd) const;
		void setENVs() const;

		std::string getMethod() const;
		std::string getUrl() const;
		const std::map<std::string, std::string>& getHeaders() const;
		std::string getBody() const;
		std::string getHeader(std::string headerName) const;
		int 		getStatusCode() const;
		std::string	getStatusCodeString() const;
		std::string getStatusMessage() const;

		void setMethod(std::string body);
		void setUrl(std::string body);
		void setHeaders(const std::map<std::string, std::string>& headers);
		void setBody(std::string body);
		void setHeader(std::string headerName, std::string headerValue);
		void setHeader(std::string fullValue);
		void setStatusCode(int code);
		void setStatusMessage(std::string code);
		void printdebug() const;
		void sendChunkedData(int fd, const std::string& body) const;
	private:
		void parseRequest(const std::map<std::string, std::string> &clMap);
		std::string reformat_request(const std::string& input);

		std::string _method;
		int			_status_code;
		std::string _status_message;
		std::string _url;
		std::map<std::string, std::string> _headers;
		std::string _body;
};

#endif
