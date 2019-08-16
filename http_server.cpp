// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <chrono>
#include <string>
#include <fstream>
#include <sstream>
#define PORT 8080

int readUntilHttpEnd(int sock, char* dest, int size){
	int totalsize = 0;
	while(true){
		auto valread = read(sock, dest, size);
		if(valread == 0){
			break;
		}
		totalsize += valread;
		if (dest[valread - 1] == '\n' && dest[valread - 2] == '\r' && dest[valread - 3] == '\n' && dest[valread - 4] == '\r'){
			break;
		}
		size -= valread;
		dest += valread;
	}
	return totalsize;
}

std::string getfile(std::string get_str){
	std::string file_dir = "/home/alexander/projects/www";
	if (get_str == "/")
		file_dir.append("/index.html");
	else
		file_dir.append(get_str);
	return file_dir;
}

std::string http_200(std::string doc, std::string type = "text/html"){
	char a[64];
	
	std::string response = "HTTP/1.1 200 OK\r\n";
	snprintf(a, sizeof(a), "Content-Length: %lu\r\n", doc.size());
	response.append(a);
	response.append("Content-Type: ");
	response.append(type);
	response.append("; charset: utf-8\r\n");
	response.append("X-Powered-By: HTTP-W/0.1\r\n");
	response.append("Connection: close\r\n");
	response.append("\r\n");
	response.append(doc);

	return response;
}

std::string http_404(){
	std::string response = "HTTP/1.0 404 Not Found\r\n";
	response.append("Content-Length: 0\r\n");
	response.append("Content-Type: text/plain\r\n");
	response.append("X-Powered-By: HTTP-W/0.1\r\n");
	response.append("Connection: close\r\n");
	response.append("\r\n");
	return response;
}

int main(int argc, char const *argv[]){
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = {0};

	// Creating socket file descriptor
	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// Forcefully attaching sokcet to the port 8080
	if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if(listen(server_fd, 3) < 0){
		perror("listen");
		exit(EXIT_FAILURE);
	}
	for(;;){
		if((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0){
			perror("accpet");
			exit(EXIT_FAILURE);
		}
		// printf("Connecting from %s\n", inet_ntoa(address.sin_addr));
		valread = readUntilHttpEnd(new_socket, buffer, sizeof(buffer));
		
		if(valread > 0){
			std::string get_str;
			std::string getdata_str;
			char tempget[1] = {0};
			char *get = &buffer[4];
			auto filerequest = strchr(get, ' ');
			auto getdata = strchr(get, '?');
			if(getdata){
				*getdata = 0;
				getdata ++;
				getdata_str = getdata;
			}
			else{
				getdata = tempget;
			}
			*filerequest = 0;

			get_str = get;
			// printf("\t%s\n", get);
			// printf("\t%s\n", getdata);

			std::string file_dir = getfile(get_str);
			
			std::ifstream infile(file_dir);
			std::string response;

			char s[1000];
			time_t tt = time(NULL);
			struct tm * p = localtime(&tt);

			strftime(s, 1000, "[%a %b %T %Y]", p);

			printf("%s %s:%5u ", s, inet_ntoa(address.sin_addr), address.sin_port);
			if(!infile){
				printf("\33[1;31m");
				printf("[404]: %s - No such file or directory\n", get);
				printf("\33[0m");
				response = http_404();
			}
			else{
				std::stringstream fbuffer;
				fbuffer << infile.rdbuf();
				std::string fstr = fbuffer.str();
				printf("\33[1;32m");
				printf("[200]: %s\n", get);
				printf("\33[0m");
				fflush(0);
				if(get_str == "/favicon.ico")
					response = http_200(fstr, "image/x-icon");
				else
					response = http_200(fstr);
			}
			infile.close();
			send(new_socket, response.c_str(), response.size(), 0);
		}
		close(new_socket);
		// printf("Connection Closed\n");
		// printf("-------------------------------\n");
	}
}