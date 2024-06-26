#pragma once

#include <netinet/in.h>

class tcp_server {
public:
	tcp_server(const char* ip, uint16_t port);

	void do_accept();

	~tcp_server();

private:
	int _sockfd;
	struct sockaddr_in _connaddr;
	socklen_t _addrlen;
};
