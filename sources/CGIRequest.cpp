#include "../includes/CGIRequest.hpp"

CGIRequest::CGIRequest() {
    _method = "";
    _url = "";
    _body = "";
}

CGIRequest::CGIRequest(const std::map<std::string, std::string> &clMap) {
    parseRequest(clMap);
}
CGIRequest::~CGIRequest() {}

void CGIRequest::send(int fd) const {
	std::string frequest;
	frequest.append("HTTP/1.1 " + getStatusCodeString() + " " + getStatusMessage() + "\r\n");
	std::map<std::string, std::string>::const_iterator it = _headers.begin();
    while (it != _headers.end()) {
		frequest.append(it->first + ": " + it->second + "\r\n");
        it++;
    }
	frequest.append("\r\n" + getBody());
    std::cout << "finalRequest: \n" << frequest << std::endl;
	write(fd, frequest.c_str(), frequest.length());
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
        else if(it->first == "Content-Length"){
            if(_method == "POST")
                setenv("CONTENT_LENGTH", (it->second).c_str(), 1);
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
        else if(it->first == "Content")
            _body = it->second;
        else 
            _headers[it->first] = it->second;
    }
}

// void CGIRequest::parseRequest(const std::string& request) {
//     std::string fRequest = reformat_request(request);
//     std::istringstream stream(fRequest);
//     std::string line;

//     if (std::getline(stream, line)) {
//         std::istringstream lineStream(line);
//         lineStream >> _method;
//         lineStream >> _url;
//         lineStream >> _version;
//     }

//     while (std::getline(stream, line) && !line.empty() && line != "\r") {
//         size_t colonPos = line.find(": ");
//         if (colonPos != std::string::npos) {
//             std::string headerName = line.substr(0, colonPos);
//             std::string headerValue = line.substr(colonPos + 2);
//             _headers[headerName] = headerValue;
//         }
//     }

//     if (std::getline(stream, line))
//         _body = line;
// }

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
