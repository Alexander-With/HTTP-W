// HTTP SERVER
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

#include "http_server.h"

#define PORT 8080

int main(int argc, char const *argv[])
{	
	int client;
	int server_fd = createSocket(PORT);

	for (;;){
        struct sockaddr_in address;
        int addrlen = sizeof(address);
        int valread;
        char buffer[1024] = {0};

        if ((client = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
		{
			perror("accpet");
			exit(EXIT_FAILURE);
		}

        valread = readUntilHttpEnd(client, buffer, sizeof(buffer));

        if (valread > 0)
        {
            std::string response = getFile(buffer, address);
            send(client, response.c_str(), response.size(), 0);
        }
		close(client);
	}
    close(server_fd);
}