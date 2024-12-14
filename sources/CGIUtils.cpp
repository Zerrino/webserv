#include "../includes/CGIUtils.hpp"

int CGIchecker(std::map<std::string, std::string> clMap, std::string FILE_PATH, setOfRuleHTTP rules, int fd){
    CGIRequest mainRequest = CGIRequest(clMap);
    mainRequest.setUrl(FILE_PATH);
    CGI cgi(rules.fastcgi_param, mainRequest, fd);
    return cgi.execute();
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

int stoi(std::string s) {
    int i;
    std::istringstream(s) >> i;
    return i;
}

