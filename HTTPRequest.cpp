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

void HTTPRequest::parseRequest(const std::string& request) {
    std::istringstream stream(request);
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

std::string HTTPRequest::getContentType() const {
    std::map<std::string, std::string>::const_iterator it = _headers.find("Content-Type");
    if (it != _headers.end()) {
        return it->second;
    }
    return "";
}

std::string HTTPRequest::getContentLength() const {
    std::map<std::string, std::string>::const_iterator it = _headers.find("Content-Length");
    if (it != _headers.end()) {
        return it->second;
    }
    return "";
}

std::string HTTPRequest::getMethod() const { return _method; }
std::string HTTPRequest::getUrl() const { return _url; }
std::string HTTPRequest::getVersion() const { return _version; }
const std::map<std::string, std::string>& HTTPRequest::getHeaders() const { return _headers; }
std::string HTTPRequest::getBody() const { return _body; }

void HTTPRequest::setMethod(std::string method) { _method = method; }
void HTTPRequest::setUrl(std::string url) { _url = url; }
void HTTPRequest::setVersion(std::string version) { _version = version; }
void HTTPRequest::setHeaders(const std::map<std::string, std::string>& headers) { _headers = headers; }
void HTTPRequest::setBody(std::string body) { _body = body; }
