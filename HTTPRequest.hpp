#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>
#include <sstream>
#include <unistd.h>
#include <iostream>
# include "webserv.hpp"

class HTTPRequest {
	public:
		HTTPRequest();
		HTTPRequest(const std::string& request);
		~HTTPRequest();

		void send(int fd) const;
		void setENVs() const;

		std::string getMethod() const;
		std::string getUrl() const;
		std::string getVersion() const;
		const std::map<std::string, std::string>& getHeaders() const;
		std::string getBody() const;
		std::string getHeader(std::string headerName) const;
		int 		getStatusCode() const;
		std::string	getStatusCodeString() const;
		std::string getStatusMessage() const;

		void setMethod(std::string body);
		void setUrl(std::string body);
		void setVersion(std::string body);
		void setHeaders(const std::map<std::string, std::string>& headers);
		void setBody(std::string body);
		void setHeader(std::string headerName, std::string headerValue);
		void setHeader(std::string fullValue);
		void setStatusCode(int code);
		void setStatusMessage(std::string code);
	private:
		void parseRequest(const std::string& request);

		std::string _method;
		int			_status_code;
		std::string _status_message;
		std::string _url;
		std::string _version;
		std::map<std::string, std::string> _headers;
		std::string _body;
};

#endif