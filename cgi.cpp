#include <iostream>
#include <cstdlib>     // pour getenv
#include <string>      // pour std::string
#include <unistd.h>    // pour dup2, execlp
#include <sys/wait.h>  // pour waitpid
#include <map>
#include <sstream>  // for std::istringstream

int runcgi(int fd) {
	std::string method = "GET";
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
            if (execlp("php-cgi", "php-cgi", "/var/www/html/info.php", (char*)NULL) == -1) {
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
			std::cout << result << std::endl;
			write(fd, result.c_str(), result.length());
        } else {
            std::cerr << "fork failed\n";
            return 1;
        }
    }

    std::cout << "Parsed request and set environment variables for CGI.\n";
    return 0;
}
