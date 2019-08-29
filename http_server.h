#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include <fstream>
#include <sstream>

#include "http_response.h"
#include "http_mime.h"

struct verbose_struct {
    std::string text;
    std::string color;
    struct sockaddr_in address;
};

int createSocket(int port);
int readUntilHttpEnd(int sock, char *dest, int size);
std::string fetchFile(std::string get_str);
void verbose(verbose_struct v);
std::string getFile(char *buffer, sockaddr_in address);