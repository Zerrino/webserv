#include "../includes/CGIRequest.hpp"
# include "../includes/CGIUtils.hpp"

CGIRequest::CGIRequest() {
    _method = "";
    _url = "";
    _body = "";
    _status_code = 0;
}

CGIRequest::CGIRequest(const std::map<std::string, std::string> &clMap) {
    parseRequest(clMap);
}
CGIRequest::~CGIRequest() {}

void CGIRequest::sendChunkedData(int fd, const std::string& body) const {
    size_t offset = 0;
    size_t chunkSize = 4096;
    while (offset < body.length()) {
        size_t toWrite = (offset + chunkSize > body.length()) ? body.length() - offset : chunkSize;
        char chunkSizeHex[16];
        snprintf(chunkSizeHex, sizeof(chunkSizeHex), "%zx", toWrite);
        write(fd, chunkSizeHex, strlen(chunkSizeHex));
        write(fd, "\r\n", 2);
        write(fd, body.c_str() + offset, toWrite);
        write(fd, "\r\n", 2);
        offset += toWrite;
    }
    write(fd, "0\r\n\r\n", 5);
}

void CGIRequest::send(int fd) const
{
    std::string frequest;
    std::string status = getStatusCodeString();
    std::string message = getStatusMessage();
    if(_body.empty()){
        if(_status_code == 0){
            status = intToString(204);
            message = "No Content";
        }
    }
    frequest.append("HTTP/1.1 " + status + " " + message + "\r\n");

    std::map<std::string, std::string>::const_iterator it = _headers.begin();
    bool useChunked = (getenv("TRANSFER_ENCODING") != NULL);

    while (it != _headers.end()) {
        if (it->first != "Content-Length" && it->first != "Transfer-Encoding")
            frequest.append(it->first + ": " + it->second + "\r\n");
        it++;
    }

    if(_body.empty()){
        frequest.append("Content-Length: 0\r\n");
    } else {
        if (useChunked)
            frequest.append("Transfer-Encoding: chunked\r\n");
        else
            frequest.append("Content-Length: " + intToString(getBody().length()) + "\r\n");
    }

    frequest.append("\r\n");

    write(fd, frequest.c_str(), frequest.length());

    if (!_body.empty()) {
        if (useChunked)
            sendChunkedData(fd, getBody());
        else
            write(fd, getBody().c_str(), getBody().length());
    }
}


void CGIRequest::setENVs() const {
    setenv("REQUEST_METHOD", _method.c_str(), 1);
    setenv("REDIRECT_STATUS", "", 1);
    setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
    setenv("PATH_INFO", _url.c_str(), 1);
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
        else if(it->first == "Content-Length") {
            if (_method == "POST" || _method == "PUT") {
                setenv("CONTENT_LENGTH", intToString(_body.length()).c_str(), 1);
            }
        } else if(it->first == "Cookie_ID") {
            setenv("HTTP_COOKIE", it->second.c_str(), 1);
        } else if(it->first == "Transfer-Encoding") {
            setenv("TRANSFER_ENCODING", it->second.c_str(), 1);
        } else if(it->first == "X-Secret-Header-For-Test") {
            setenv("HTTP_X_SECRET_HEADER_FOR_TEST", it->second.c_str(), 1);
        }
        it++;
    }
}

std::string CGIRequest::reformat_request(const std::string& input) {
    std::string result = "";
    for (size_t i = 0; i < input.length(); ++i) {
        if (input[i] != '\r')
            result += input[i];
    }
    return result;
}

void CGIRequest::parseRequest(const std::map<std::string, std::string> &clMap) {
    for (std::map<std::string, std::string>::const_iterator it = clMap.begin(); it != clMap.end(); ++it) {
        if(it->first == "METHOD")
            _method = it->second;
        else if(it->first == "URI")
            _url = it->second;
        else if(it->first == "Content"){
            _body = it->second;
        } else
            _headers[it->first] = it->second;
    }
}

std::string CGIRequest::getHeader(std::string headerName) const {
    std::map<std::string, std::string>::const_iterator it = _headers.find(headerName);
    if (it != _headers.end())
        return it->second;
    return "";
}

void CGIRequest::setHeader(std::string headerName, std::string headerValue){
    std::map<std::string, std::string>::const_iterator it = _headers.find(headerName);
	if (it != _headers.end())
		_headers[headerName] = headerValue;
	else
		_headers.insert(std::make_pair(headerName, headerValue));
}

void CGIRequest::setHeader(std::string fullValue){
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


std::string CGIRequest::getMethod() const { return _method; }
std::string CGIRequest::getUrl() const { return _url; }
const std::map<std::string, std::string>& CGIRequest::getHeaders() const { return _headers; }
std::string CGIRequest::getBody() const { return _body; }
int CGIRequest::getStatusCode() const { return _status_code; }
std::string CGIRequest::getStatusCodeString() const {
	std::stringstream ss;
	ss << _status_code;
	return ss.str();
}
std::string CGIRequest::getStatusMessage() const { return _status_message; }

void CGIRequest::setMethod(std::string method) { _method = method; }
void CGIRequest::setUrl(std::string url) { _url = url; }
void CGIRequest::setHeaders(const std::map<std::string, std::string>& headers) { _headers = headers; }
void CGIRequest::setBody(std::string body) { _body = body; }
void CGIRequest::setStatusCode(int code) { _status_code = code; }
void CGIRequest::setStatusMessage(std::string message) { _status_message = message; }

void CGIRequest::printdebug() const {
    std::cout << "CGIRequest Debug Information:" << std::endl;
    std::cout << "Method: " << _method << std::endl;
    std::cout << "URL: " << _url << std::endl;
    if(_body.length() < 1000)
        std::cout << "Body: " << _body << std::endl;
    std::cout << "Headers:" << std::endl;

    std::map<std::string, std::string>::const_iterator it = _headers.begin();
    while (it != _headers.end()) {
        std::cout << "  " << it->first << ": " << it->second << std::endl;
        it++;
    }

    std::cout << "Status Code: " << getStatusCode() << std::endl;
    std::cout << "Status Message: " << getStatusMessage() << std::endl;
}
