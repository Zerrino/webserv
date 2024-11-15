#include "CGI.hpp"

CGI::CGI(const std::string type, const HTTPRequest& request, int fd): _type(type), _request(request), _fd(fd) {}

CGI::~CGI() {}


int CGI::sendPostBody(int pipe_in[2]){
	if (_request.getMethod() == "POST") {
		ssize_t bytesWritten = write(pipe_in[1], _request.getBody().c_str(), _request.getBody().length());
		if (bytesWritten == -1) {
			std::cerr << "Failed to write to pipe_in\n";
			return 1;
		}
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


int CGI::execute() {
    if (_type != "PHP" && _type != "PYTHON")
        return 1;

    std::string localPath = "/var/www/html";
    std::string fileToExecute = _request.getUrl().substr(1);
    std::string cmd;
    if(_type == "PHP") cmd = "php-cgi";
	else if(_type == "PYTHON") cmd = "python3";

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
