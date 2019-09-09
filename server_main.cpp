#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "http_server.h"
#include "http_ssl.h"

int main(int argc, char const *argv[]){
	configuration conf = get_config();

	int client;
	int server_fd = createSocket(conf.port, conf.conn_max);

	if(conf.req_https){
		printf("Running in HTTPS\n");
		SSL_CTX *ctx;
		init_openssl();
		ctx = create_context();
		// configure_context(ctx, conf.https_cert, conf.https_key);
		configure_context(ctx);
		for (;;)
		{
			struct sockaddr_in address;
			int addrlen = sizeof(address);
			int valread;
			char buffer[1024] = {0};

			SSL *ssl;

			if ((client = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
			{
				perror("accpet");
				exit(EXIT_FAILURE);
			}

			ssl = SSL_new(ctx);
			SSL_set_fd(ssl, client);

			ERR_clear_error();
			auto ret = SSL_accept(ssl);
			if (ret <= 0){
				ssl_show_error_stack();
			}
			else
			{
				valread = SSL_read(ssl, buffer, sizeof(buffer));

				if (valread > 0)
				{
					std::string response = getFile(buffer, address);
					SSL_write(ssl, response.c_str(), response.size());
				}
			}
			SSL_shutdown(ssl);
			SSL_free(ssl);
			close(client);
		}
		close(server_fd);
		SSL_CTX_free(ctx);
		cleanup_openssl();
	}
	else{
		printf("Running in HTTP\n");
		for (;;)
		{
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
}