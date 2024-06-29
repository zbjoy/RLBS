#pragma once

#include <netinet/in.h>
#include "event_loop.h"

class tcp_server {
public:
	tcp_server(event_loop* loop, const char* ip, uint16_t port);

	void do_accept();

	~tcp_server();

private:
	int _sockfd;
	struct sockaddr_in _connaddr;
	socklen_t _addrlen;

	event_loop* _loop;
};
