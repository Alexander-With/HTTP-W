#include "http_server.h"

int createSocket(int port){
	int server_fd;
	struct sockaddr_in address;
	int opt = 1;

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	// Forcefully attaching sokcet to the port 8080
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	return server_fd;
}

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

std::string fetchFile(std::string get_str){
	std::string file_dir = "www";
	if (get_str == "/")
		file_dir.append("/index.html");
	else
		file_dir.append(get_str);
	return file_dir;
}

void verbose(verbose_struct v){
	char s[1000];
	time_t tt = time(NULL);
	struct tm *p = localtime(&tt);
	strftime(s, 1000, "[%a %b %T %Y]", p);

	printf("%s %s:%5u ", s, inet_ntoa(v.address.sin_addr), v.address.sin_port);
	printf("%s", v.color.c_str());
	printf("%s", v.text.c_str());
	printf("\x1b[0m\n");
}

std::string getFile(char* buffer, sockaddr_in address){
	std::string getdata_str;
	char tempget[1] = {0};
	char *get = &buffer[4];

	auto filerequest = strchr(get, ' ');
	auto getdata = strchr(get, '?');
	if (getdata)
	{
		*getdata = 0;
		getdata++;
		getdata_str = getdata;
	}
	else
	{
		getdata = tempget;
	}
	*filerequest = 0;

	std::string file_dir = fetchFile((std::string)get);

	std::ifstream infile(file_dir);
	std::string response;

	verbose_struct v;
	v.address = address;
	if (!infile)
	{
		// Verbose to Console
		v.color = "\e[31m";
		v.text = "[404]: " + (std::string)get + " - No such file or directory";
		verbose(v);

		response = http_404();
	}
	else
	{
		std::stringstream fbuffer;
		fbuffer << infile.rdbuf();
		std::string fstr = fbuffer.str();
		// Verbose to Console
		v.color = "\e[32m";
		v.text = "[200]: " + (std::string)get;
		verbose(v);

		fflush(0);
		std::string mime = getmime((std::string)file_dir);
		response = http_200(fstr, mime);
	}
	infile.close();

	return response;
}