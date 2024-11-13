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
	HTTPRequest request = HTTPRequest(getTestRequest());
	std::string method = request.getMethod();
	std::string localPath = "/var/www/html";
	std::string fileToExecute = localPath + request.getUrl();

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

			close(pipe_in[0]);
			close(pipe_out[1]);

			setenv("REQUEST_METHOD", method.c_str(), 1);
			setenv("SCRIPT_FILENAME", fileToExecute.c_str(), 1);
			setenv("REDIRECT_STATUS", "", 1);
			setenv("CONTENT_TYPE", request.getHeader("Content-Type").c_str(), 1);
			if (method == "POST")
				setenv("CONTENT_LENGTH", (request.getHeader("Content-Length")).c_str(), 1);

            if (execlp("php-cgi", "php-cgi", fileToExecute, (char*)NULL));
			exit(1);
        } else if (pid > 0) {
			close(pipe_in[0]);
			close(pipe_out[1]);
			if (method == "POST")
         	   write(pipe_in[1], request.getBody().c_str(), request.getBody().length());
			close(pipe_in[1]);

			char buffer[4096];
			std::string result;
			ssize_t bytesRead;
			while ((bytesRead = read(pipe_out[0], buffer, sizeof(buffer) - 1)) > 0) {
				buffer[bytesRead] = '\0';
				result += buffer;
			}
			close(pipe_out[0]);
			std::cout << "result" << std::endl;
			std::cout << result << std::endl;
			std::cout << std::endl;
			int status;
			waitpid(pid, &status, 0);
			std::string frequest = request.getVersion()+" 200 OK\r\n";
			frequest.append(getDate()+"\r\n");
			std::string header = result.substr(0, result.find("\n"));
			frequest.append(header+"\r\n");
			std::stringstream ss;
			ss << result.length();
			std::string lengthStr = ss.str();
			frequest.append(lengthStr+"\r\n");
			frequest.append("\r\n\r\n");
			result = result.substr(result.find("\n") + 1, result.length() - 1);
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
