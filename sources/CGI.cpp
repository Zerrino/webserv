#include "../includes/CGI.hpp"

CGI::CGI(const std::string cmd, const CGIRequest& request, int fd): _cmd(cmd), _request(request), _fd(fd) {}

CGI::~CGI() {}
#include <signal.h> //TODO
#include <sys/select.h> //TODO
int CGI::sendPostBody(int pipe_in[2]) {
    ssize_t bytesWritten = write(pipe_in[1], _request.getBody().c_str(), _request.getBody().length());
    if (bytesWritten == -1) {
        perror("Erreur d'écriture dans pipe");
        return 1;
    }
    std::cout << "Requête envoyée via pipe\n";
    return 0;
}

bool CGI::createPipes(int pipe_in[2], int pipe_out[2], int pipe_signal[2]) {
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

void setStatus(std::string str, int &code, std::string &msg){
    size_t spacePos = str.find(' ');
    code = stoi(str.substr(0, spacePos));
    msg = str.substr(spacePos + 1);
}

void CGI::sendResponse(const std::string& result) const {
    std::map<std::string, std::string> headers;
    std::string body;
    int status_code = 200;
    std::string status_message = "OK";
    parseResult(result, headers, body);

    if(headers.size() > 0){
        if (headers.find("Status") != headers.end()){
            setStatus(headers["Status"], status_code, status_message);
            headers.erase("Status");
        }
    }

    CGIRequest resultRequest;
    resultRequest.setStatusCode(status_code);
    resultRequest.setStatusMessage(status_message);
    resultRequest.setHeaders(headers);
    resultRequest.setBody(body);
    resultRequest.send(_fd);
}

#include <iostream>
int CGI::execute() {
    std::string localPath, fileToExecute;
    splitPath(_request.getUrl(), localPath, fileToExecute);

    const std::string tempFilePath = "tmp/webserv.txt";
    int isPiped = -1;
    if(_request.getMethod() == "POST" || _request.getMethod() == "PUT")
        isPiped = 0;
    long pipe_max_size = 100000;
    if (_request.getBody().length() > pipe_max_size) {
        std::ofstream tempFile(tempFilePath.c_str());
        if (!tempFile) {
            std::cerr << "Erreur lors de la création du fichier temporaire.\n";
            return 1;
        }
        tempFile << _request.getBody();
        tempFile.close();
    } else
        isPiped = 1;

    int pipe_in[2], pipe_out[2], pipe_signal[2];
    if (!createPipes(pipe_in, pipe_out, pipe_signal)) {
        std::cerr << "Failed to create pipes.\n";
        return 1;
    }
    std::cout << "cmd: " << _cmd << std::endl;
    _request.printdebug();
    pid_t pid = fork();
    if (pid == 0) {
        close(pipe_in[1]);
        close(pipe_out[0]);

        dup2(pipe_in[0], STDIN_FILENO);
        dup2(pipe_out[1], STDOUT_FILENO);

        close(pipe_in[0]);
        close(pipe_out[1]);

        if(isPiped == 0){
            int fd = open(tempFilePath.c_str(), O_RDONLY);
            dup2(fd, STDIN_FILENO);
        }

        chdir(localPath.c_str());
        setEnvironmentVariables(localPath, fileToExecute);

        execlp(_cmd.c_str(), _cmd.c_str(), fileToExecute.c_str(), (char*)NULL);
        perror("execlp failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        close(pipe_in[0]);
        close(pipe_out[1]);

        if(isPiped == 1)
            if (sendPostBody(pipe_in) == 1)
                return 1;
        close(pipe_in[1]);

        char buffer[4096];
        std::string result;
        ssize_t bytesRead;
        while ((bytesRead = read(pipe_out[0], buffer, sizeof(buffer))) > 0) {
            result.append(buffer, bytesRead);
        }
        std::cout << "Printing result" << std::endl;
        // if(isPiped == 1 || isPiped == 0){
        //     const std::string outFilePath = "tmp/out.txt";
        //     std::ofstream outFile(outFilePath.c_str());
        //     if (!outFile) {
        //         std::cerr << "Erreur lors de la création du fichier temporaire.\n";
        //         return 1;
        //     }
        //     outFile << result;
        //     outFile.close();
        // }
        close(pipe_out[0]);
        int status;
        waitpid(pid, &status, 0);
        sendResponse(result);
    } else {
        std::cerr << "fork failed\n";
        return 1;
    }
    if(isPiped == 0)
        std::remove(tempFilePath.c_str());
    return 0;
}