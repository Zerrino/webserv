#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest() {
    _method = "";
    _url = "";
    _version = "";
    _body = "";
}

HTTPRequest::HTTPRequest(const std::string& request) {
    parseRequest(request);
}
HTTPRequest::~HTTPRequest() {}

void HTTPRequest::send(int fd) const {
	std::string frequest;
	frequest.append(getVersion() + " " + getStatusCodeString() + " " + getStatusMessage() + "\r\n");
	std::map<std::string, std::string>::const_iterator it = _headers.begin();
    while (it != _headers.end()) {
		frequest.append(it->first + ": " + it->second + "\r\n");
        it++;
    }
	frequest.append("\r\n" + getBody());
	write(fd, frequest.c_str(), frequest.length());
}

void HTTPRequest::setENVs() const {
    setenv("REQUEST_METHOD", _method.c_str(), 1);
    setenv("REDIRECT_STATUS", "", 1);
    std::map<std::string, std::string>::const_iterator it = _headers.begin();
    while (it != _headers.end()) {
		if(it->first == "User-Agent")
            setenv("HTTP_USER_AGENT", (it->second).c_str(), 1);
        else if(it->first == "Connection")
            setenv("HTTP_CONNECTION", (it->second).c_str(), 1);
        else if(it->first == "Host")
            setenv("HTTP_HOST", (it->second).c_str(), 1);
        else if(it->first == "Content-Type")
            setenv("CONTENT_TYPE", (it->second).c_str(), 1);
        else if(it->first == "Content-Length"){
            if(_method == "POST")
                setenv("CONTENT_LENGTH", (it->second).c_str(), 1);
        }
        it++;
    }
}

std::string HTTPRequest::reformat_request(const std::string& input) {
    std::string result = "";
    for (size_t i = 0; i < input.length(); ++i) {
        if (input[i] == '\r' && i + 1 < input.length() && input[i + 1] == '\n') {
            result += '\n';
            ++i;
        } else result += input[i];
    }
    return result;
}

void HTTPRequest::parseRequest(const std::string& request) {
    std::string fRequest = reformat_request(request);
    std::istringstream stream(fRequest);
    std::string line;

    if (std::getline(stream, line)) {
        std::istringstream lineStream(line);
        lineStream >> _method;
        lineStream >> _url;
        lineStream >> _version;
    }

    while (std::getline(stream, line) && !line.empty() && line != "\r") {
        size_t colonPos = line.find(": ");
        if (colonPos != std::string::npos) {
            std::string headerName = line.substr(0, colonPos);
            std::string headerValue = line.substr(colonPos + 2);
            _headers[headerName] = headerValue;
        }
    }

    if (std::getline(stream, line))
        _body = line;
}

std::string HTTPRequest::getHeader(std::string headerName) const {
    std::map<std::string, std::string>::const_iterator it = _headers.find(headerName);
    if (it != _headers.end()) {
        return it->second;
    }
    return "";
}

void HTTPRequest::setHeader(std::string headerName, std::string headerValue){
std::map<std::string, std::string>::const_iterator it = _headers.find(headerName);
	if (it != _headers.end()) 
		_headers[headerName] = headerValue;
	else 
		_headers.insert(std::make_pair(headerName, headerValue));
}

void HTTPRequest::setHeader(std::string fullValue){
	std::string key;
	std::string value;
	size_t colonPos = fullValue.find(":");
    if (colonPos != std::string::npos) {
        key = fullValue.substr(0, colonPos);
		value = fullValue.substr(colonPos + 1);
		size_t start = value.find_first_not_of(" \t");
        size_t end = value.find_last_not_of(" \t");
        if (start != std::string::npos && end != std::string::npos)
            value = value.substr(start, end - start + 1);
    }
	setHeader(key, value);
}


std::string HTTPRequest::getMethod() const { return _method; }
std::string HTTPRequest::getUrl() const { return _url; }
std::string HTTPRequest::getVersion() const { return _version; }
const std::map<std::string, std::string>& HTTPRequest::getHeaders() const { return _headers; }
std::string HTTPRequest::getBody() const { return _body; }
int HTTPRequest::getStatusCode() const { return _status_code; }
std::string HTTPRequest::getStatusCodeString() const { 
	std::stringstream ss;
	ss << _status_code;
	return ss.str();
}
std::string HTTPRequest::getStatusMessage() const { return _status_message; }

void HTTPRequest::setMethod(std::string method) { _method = method; }
void HTTPRequest::setUrl(std::string url) { _url = url; }
void HTTPRequest::setVersion(std::string version) { _version = version; }
void HTTPRequest::setHeaders(const std::map<std::string, std::string>& headers) { _headers = headers; }
void HTTPRequest::setBody(std::string body) { _body = body; }
void HTTPRequest::setStatusCode(int code) { _status_code = code; }
void HTTPRequest::setStatusMessage(std::string message) { _status_message = message; }
