#include <iostream>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "rlbs_reactor.h"
#include "tcp_server.h"

void rlbs_hello()
{
	std::cout << "RLBS hello......" << std::endl;	
}

tcp_server::tcp_server(const char* ip, uint16_t port)
{	
	if (signal(SIGHUP, SIG_IGN))
	{
		fprintf(stderr, "signal ignore SIGHUP\n");			
	}

	if (signal(SIGPIPE, SIG_IGN))
	{
		fprintf(stderr, "signal ignore SIGPIPE");		
	}

	_sockfd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
	if (_sockfd == -1)
	{
		fprintf(stderr, "tcp::server: socket()\n");		
		exit(1);
	}

	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	// inet_aton
	inet_pton(AF_INET, ip, &server_addr.sin_addr.s_addr);
	server_addr.sin_port = htons(port);

	int opt = 1;
	if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
	{
		fprintf(stderr, "setsockopt reuse error\n");		
	}


	if (bind(_sockfd, (const struct sockaddr*)&server_addr, sizeof(server_addr)))
	{
		fprintf(stderr, "bind error\n");		
		exit(1);
	}

	if (listen(_sockfd, 500) == -1)
	{
		fprintf(stderr, "listen error\n");		
		exit(1);
	}

}

void tcp_server::do_accept()
{
	int connfd;	
	while (true)
	{
		connfd = accept(_sockfd, (struct sockaddr*)&_connaddr, &_addrlen);
		if (connfd == -1)
		{
			if (errno == EINTR)		
			{
				fprintf(stderr, "accept error is EINTR");
				continue;
			}
			else if (errno == EAGAIN)
			{
				fprintf(stderr, "accept error is EAGAIN");
				continue;
				
			}
			else if (errno == EMFILE)
			{
				fprintf(stderr, "accept error is EMFILE");
				continue;
			}
			else
			{
				fprintf(stderr, "accept error is ");
				exit(1);
			}
		}
		else
		{
			//TODO 添加心跳机制
			//TODO 添加消息队列


			// 回显
			int writed;
			const char* data = "hello rlbs!!\n";

			do {
				// writed = write(connfd, data, sizeof(data));
				writed = write(connfd, data, strlen(data) + 1);
			} while(writed == -1 && errno == EINTR);

			if (writed > 0)
			{
				printf("write success!!\n");		
			}
		}
	}
}


tcp_server::~tcp_server()
{
	close(_sockfd);
}
