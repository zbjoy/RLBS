#pragma once

#include <netinet/in.h>
#include "net_connection.h"
#include "event_loop.h"
#include "message.h"

class udp_server : public net_connection
{
public:
	udp_server(event_loop* loop, const char* ip, uint16_t port);

	virtual int send_message(const char* data, int msglen, int msgid);

	void add_msg_router(int msgid, msg_callback* cb, void* user_data = NULL);

	~udp_server();

	// 处理客户端数据的业务
	void do_read();

private:
	int _sockfd;

	event_loop* _loop;

	char _read_buf[MESSAGE_LENGTH_LIMIT];
	char _write_buf[MESSAGE_LENGTH_LIMIT];

	struct sockaddr_in _client_addr;
	socklen_t _client_addrlen;

	msg_router _router;

};
