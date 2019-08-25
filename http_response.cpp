#include "http_response.h"

#define X_POWERED_BY "X-Powered-By: HTTP-W/0.2";
#define CHARSET "charset: utf-8";
#define CONNECTION "Connection: close";

std::string http_200(std::string doc, std::string type){
    char a[64];
    std::string t;

    std::string response = "HTTP/1.1 200 OK\r\n";
    snprintf(a, sizeof(a), "Content-Length: %lu\r\n", doc.size());
    response.append(a);
    response.append("Content-Type: " + type + "\r\n");
    t = CHARSET;
    response.append(t + "\r\n");
    t = X_POWERED_BY;
    response.append(t + "\r\n");
    t = CONNECTION;
    response.append(t + "\r\n");
    response.append("\r\n");
    response.append(doc);

    return response;
}

std::string http_404(){
    std::string t;

    std::string response = "HTTP/1.0 404 Not Found\r\n";
    response.append("Content-Length: 0\r\n");
    response.append("Content-Type: text/plain\r\n");
    t = X_POWERED_BY;
    response.append(t + "\r\n");
    t = CONNECTION;
    response.append(t + "\r\n");
    response.append("\r\n");
    return response;
}