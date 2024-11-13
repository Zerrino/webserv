#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>
#include <sstream>

class HTTPRequest {
	public:
		HTTPRequest();
		HTTPRequest(const std::string& request);
		~HTTPRequest();

		std::string getContentType() const;
		std::string getContentLength() const;
		
		std::string getMethod() const;
		std::string getUrl() const;
		std::string getVersion() const;
		const std::map<std::string, std::string>& getHeaders() const;
		std::string getBody() const;

		void setMethod(std::string body);
		void setUrl(std::string body);
		void setVersion(std::string body);
		void setHeaders(const std::map<std::string, std::string>& headers);
		void setBody(std::string body);

	private:
		void parseRequest(const std::string& request);

		std::string _method;
		std::string _url;
		std::string _version;
		std::map<std::string, std::string> _headers;
		std::string _body;
};

#endif