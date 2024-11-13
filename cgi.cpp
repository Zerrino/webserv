#include <iostream>
#include <cstdlib>     // pour getenv
#include <string>      // pour std::string
#include <unistd.h>    // pour dup2, execlp
#include <sys/wait.h>  // pour waitpid
#include <map>
#include <sstream>  // for std::istringstream
#include "SendToClient.hpp"
#include <map>
#include "HTTPRequest.hpp"

std::string getTestRequestPOSTPHP() {
    return "POST /info.php HTTP/1.1\n"
           "Host: example.com\n"
           "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.75 Safari/537.36\n"
           "Content-Type: application/x-www-form-urlencoded\n"
           "Content-Length: 27\n"
           "Connection: keep-alive\n\n"
           "user=alice&age=25&city=Paris";
}

std::string getTestRequestGETPYTHON() {
    return "GET /hello.py HTTP/1.1\n"
           "Host: example.com\n"
           "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.75 Safari/537.36\n"
           "Connection: keep-alive\n\n"
           "name=alice&age=30";
}


void sendBackRequest(int fd, std::string result){
	HTTPRequest resultRequest = HTTPRequest();
	resultRequest.setStatusCode(200);
	resultRequest.setStatusMessage("OK");
	resultRequest.setVersion("HTTP/1.1");
	resultRequest.setHeader(result.substr(0, result.find("\n")));
	std::stringstream ss;
	ss << result.length();
	std::string lengthStr = ss.str();
	resultRequest.setHeader("Content-Length", lengthStr);
	result = result.substr(result.find("\n") + 1, result.length() - 1);
	resultRequest.setBody(result);

	resultRequest.send(fd);
}

int runcgi(int fd, std::string requestType) {
	if(requestType != "PHP" && requestType != "PYTHON")
		return 1;
	HTTPRequest request = HTTPRequest(getTestRequestGETPYTHON());
	std::string method = request.getMethod();
	std::string localPath = "/var/www/html";
	std::string fileToExecute = request.getUrl().substr(1);

	std::string cmd;
	if(requestType == "PHP")
		cmd = "php-cgi";
	else if(requestType == "PYTHON")
		cmd = "python3";

	int pipe_in[2], pipe_out[2];
    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) {
        std::cerr << "Pipe creation failed\n";
        return 1;
    }
    if (method == "POST" || method == "GET") {
        pid_t pid = fork();
        if (pid == 0) {
			close(pipe_in[1]);
			close(pipe_out[0]);

			dup2(pipe_in[0], STDIN_FILENO);
			dup2(pipe_out[1], STDOUT_FILENO);
			if (dup2(pipe_in[0], STDIN_FILENO) == -1) {
				std::cerr << "Failed to redirect STDIN\n";
				exit(1); }
			if (dup2(pipe_out[1], STDOUT_FILENO) == -1) {
				std::cerr << "Failed to redirect STDOUT\n";
				exit(1); }

			close(pipe_in[0]);
			close(pipe_out[1]);
			
			chdir(localPath.c_str());
			if (chdir(localPath.c_str()) == -1) {
				std::cerr << "Failed to change directory\n";
				exit(1); }

			setenv("SCRIPT_FILENAME", (localPath + "/" + fileToExecute).c_str(), 1);
			request.setENVs();
            execlp(cmd.c_str(), cmd.c_str(), (fileToExecute).c_str(), (char*)NULL);
			exit(1);
        } else if (pid > 0) {
			close(pipe_in[0]);
			close(pipe_out[1]);
			if (method == "POST"){
				ssize_t bytesWritten = write(pipe_in[1], request.getBody().c_str(), request.getBody().length());
				if (bytesWritten == -1) {
					std::cerr << "Failed to write to pipe_in\n";
					return 1;
				}
			}
			close(pipe_in[1]);
			char buffer[4096];
			std::string result;
			ssize_t bytesRead;
			while ((bytesRead = read(pipe_out[0], buffer, sizeof(buffer) - 1)) > 0) {
				buffer[bytesRead] = '\0';
				result += buffer;
			}
			close(pipe_out[0]);

			int status;
			waitpid(pid, &status, 0);
			sendBackRequest(fd, result);
        } else {
            std::cerr << "fork failed\n";
            return 1;
        }
    }

    std::cout << "Parsed request and set environment variables for CGI.\n";
    return 0;
}
