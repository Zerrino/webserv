#include "CGI.hpp"

CGI::CGI(const std::string type, const HTTPRequest& request, int fd): _type(type), _request(request), _fd(fd) {}

CGI::~CGI() {}

std::string getTypeName(Types type) {
    switch (type) {
        case PHP:    return "PHP";
        case PYTHON: return "PYTHON";
        default:     return "NULL";
    }
}

std::string getTypeName(std::string suffix) {
    if (suffix == "php") return "PHP";
    if (suffix == "py") return "PYTHON";
    return "NULL";
}

int CGIchecker(std::string clientInfo, std::string PATH_ABS, int fd){
    HTTPRequest mainRequest = HTTPRequest(clientInfo);

    mainRequest.setUrl(PATH_ABS);
    size_t pos = mainRequest.getUrl().rfind('.');
    if (pos != std::string::npos){
        std::string suffix = mainRequest.getUrl().substr(pos + 1);
        if(suffix != "php" && suffix != "py")
            return 2;
        CGI cgi(getTypeName(suffix), mainRequest, fd);
        return cgi.execute();
    } else return 2;
}

int CGI::sendPostBody(int pipe_in[2]){
	if (_request.getMethod() == "POST") {
		ssize_t bytesWritten = write(pipe_in[1], _request.getBody().c_str(), _request.getBody().length());
		if (bytesWritten == -1)
			return 1;
	}
    return 0;
}

bool CGI::createPipes(int pipe_in[2], int pipe_out[2]) {
    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) {
        return false;
    }
    return true;
}

void CGI::setEnvironmentVariables(std::string localPath, std::string fileToExecute) {
    setenv("SCRIPT_FILENAME", (localPath + "/" + fileToExecute).c_str(), 1);
    _request.setENVs();
}

void CGI::parseResult(const std::string& data, std::map<std::string, std::string>& headers, std::string& body) const {
    size_t separatorPos = data.find("\r\n\r\n");

    if (separatorPos != std::string::npos) {
        std::string headerPart = data.substr(0, separatorPos);
        body = data.substr(separatorPos + 4);

        std::istringstream headerStream(headerPart);
        std::string line;
        while (std::getline(headerStream, line)) {
            if (line.empty()) continue;
            size_t colonPos = line.find(": ");
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 2);
                headers[key] = value;
            }
        }
    }
}

void CGI::sendResponse(const std::string& result) const {
    std::map<std::string, std::string> headers;
    std::string body;
    parseResult(result, headers, body);

    HTTPRequest resultRequest;
    resultRequest.setStatusCode(200);
    resultRequest.setStatusMessage("OK");
    resultRequest.setVersion("HTTP/1.1");
    resultRequest.setHeaders(headers);
    resultRequest.setBody(body);

    resultRequest.send(_fd);
}



void splitPath(const std::string& fullPath, std::string& path, std::string& file) {
    std::size_t lastSlashPos = fullPath.find_last_of("/");

    if (lastSlashPos != std::string::npos) {
        path = fullPath.substr(0, lastSlashPos);
        file = fullPath.substr(lastSlashPos + 1);
    } else {
        path = "";
        file = fullPath;
    }
}

bool isPHPInstalled() {
    return std::system("php-cgi -v > /dev/null 2>&1") == 0;
}

bool isPythonInstalled() {
    return std::system("python3 -V >nul 2>nul") == 0;
}

int CGI::execute() {
    if (_type != "PHP" && _type != "PYTHON")
        return 1;

    std::string localPath, fileToExecute;
    splitPath(_request.getUrl(), localPath, fileToExecute);

    std::string cmd;
    if(_type == "PHP"){ 
        cmd = "php-cgi";
        if(!isPHPInstalled()){
            std::cerr << "Error: php-cgi is not installed.";
            return 1;
        }
    }
	else if(_type == "PYTHON"){ 
        cmd = "python3";
        if(!isPythonInstalled()){
            std::cerr << "Error: python3 is not installed.";
            return 1;
        }
    };

    int pipe_in[2], pipe_out[2];
    if (!createPipes(pipe_in, pipe_out)) {
        std::cerr << "Failed to create pipes.\n";
        return 1;
    }
    pid_t pid = fork();
    if (pid == 0) {
        close(pipe_in[1]);
        close(pipe_out[0]);

        dup2(pipe_in[0], STDIN_FILENO);
        dup2(pipe_out[1], STDOUT_FILENO);

        close(pipe_in[0]);
        close(pipe_out[1]);

        chdir(localPath.c_str());
        setEnvironmentVariables(localPath, fileToExecute);

        execlp(cmd.c_str(), cmd.c_str(), fileToExecute.c_str(), (char*)NULL);
        exit(1);
    } else if (pid > 0) {
        close(pipe_in[0]);
        close(pipe_out[1]);

        if(sendPostBody(pipe_in) == 1)
            return 1;
        close(pipe_in[1]);

        char buffer[4096];
        std::string result;
        ssize_t bytesRead;
        while ((bytesRead = read(pipe_out[0], buffer, sizeof(buffer))) > 0) {
            result.append(buffer, bytesRead);
        }
        close(pipe_out[0]);

        int status;
        waitpid(pid, &status, 0);
        sendResponse(result);
    } else {
        std::cerr << "fork failed\n";
        return 1;
    }
    return 0;
}
