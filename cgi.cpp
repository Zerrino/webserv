#include <iostream>
#include <cstdlib>     // pour getenv
#include <string>      // pour std::string
#include <unistd.h>    // pour dup2, execlp
#include <sys/wait.h>  // pour waitpid
#include <map>
#include <sstream>  // for std::istringstream
#include "SendToClient.hpp"

std::string getTestRequest() {
    return "POST /info.php HTTP/1.1\n"
           "Host: example.com\n"
           "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.75 Safari/537.36\n"
           "Content-Type: application/x-www-form-urlencoded\n"
           "Content-Length: 27\n"
           "Connection: keep-alive\n\n"
           "user=alice&age=25&city=Paris";
}

std::string	getDate()
{
	time_t timestamp;
	char buffer[31];
	time(&timestamp);
	struct tm *gmt = std::gmtime(&timestamp);
	std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT\r\n", gmt);
	std::string str(buffer);
	std::string s = "Date: ";
	s.append(str);
	return (s);
}


int runcgi(int fd) {
	std::string request = getTestRequest();
	std::string method = request.substr(0, request.find(" "));
	std::cout << "method: " << method << std::endl;
	std::string localPath = "/var/www/html";
	int start = request.find(" ");
	int end = request.find(" ", start + 1);
	std::string fileToExecute = localPath + request.substr(start + 1, end - start - 1);
	std::cout << "filepath to execute: " << fileToExecute << std::endl;
	setenv("REQUEST_METHOD", method.c_str(), 1);
	setenv("SCRIPT_FILENAME", fileToExecute.c_str(), 1);
	setenv("REDIRECT_STATUS", "", 1);

	int pipefd[2];
    if (pipe(pipefd) == -1) {
        std::cerr << "Pipe failed\n";
        return 1;
    }
    if (method == "POST" || method == "GET") {
        pid_t pid = fork();
        if (pid == 0) {
			close(pipefd[0]);
			dup2(pipefd[1], STDOUT_FILENO);
			close(pipefd[1]);
            if (execlp("php-cgi", "php-cgi", fileToExecute, (char*)NULL) == -1) {
                exit(1);
            }
        } else if (pid > 0) {
			close(pipefd[1]);
			char buffer[4096];
			std::string result;
			ssize_t bytesRead;
			while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
				buffer[bytesRead] = '\0';
				result += buffer;
			}
			close(pipefd[0]);
			int status;
			waitpid(pid, &status, 0);
			std::string frequest = "HTTP/1.1 200 OK\r\n";
			frequest.append(getDate()+"\r\n");
			frequest.append("Content-Type: text/html; charset=utf-8\r\n");
			frequest.append(result.length()+"\r\n");
			frequest.append("\r\n\r\n");
			result = result.substr(result.find("Content-type: text/html; charset=UTF-8") + 39, result.length() - 1);
			frequest.append(result);
			std::cout << frequest << std::endl;
			write(fd, frequest.c_str(), frequest.length());
        } else {
            std::cerr << "fork failed\n";
            return 1;
        }
    }

    std::cout << "Parsed request and set environment variables for CGI.\n";
    return 0;
}
