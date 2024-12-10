#include "../includes/CGIUtils.hpp"

int CGIchecker(std::string clientInfo, std::string PATH_ABS, int fd){
    CGIRequest mainRequest = CGIRequest(clientInfo);

    mainRequest.setUrl(PATH_ABS);
    size_t pos = mainRequest.getUrl().rfind('.');
    if (pos != std::string::npos){
        std::string suffix = mainRequest.getUrl().substr(pos + 1);
        if(getTypeName(suffix) == "NULL")
            return 2;
        CGI cgi(getTypeName(suffix), getTypeCommand(suffix), mainRequest, fd);
        return cgi.execute();
    } else return 2;
}

std::string getTypeName(Types type) { //TODO to remove
    switch (type) {
        case PHP:    return "PHP";
        case PYTHON: return "PYTHON";
        default:     return "NULL";
    }
}

std::string getTypeName(std::string suffix) { //TODO to remove
    if (suffix == "php") return "PHP";
    if (suffix == "phtml") return "PHP";
    if (suffix == "py") return "PYTHON";
    return "NULL";
}

std::string getTypeCommand(std::string suffix) { //TODO to remove
    if (suffix == "php") return "php-cgi";
    if (suffix == "phtml") return "python3";
    if (suffix == "py") return "PYTHON";
    return "NULL";
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
