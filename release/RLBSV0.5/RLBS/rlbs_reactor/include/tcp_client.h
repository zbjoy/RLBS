#pragma once


#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "reactor_buf.h"
#include "event_loop.h"


class tcp_client;
typedef void msg_callback(const char* data, uint32_t len, int msgid, tcp_client* client, void* user_data);

class tcp_client
{
public:	
	tcp_client(event_loop* loop, const char* ip, unsigned short port);

	int send_message(const char* data, int msglen, int msgid);

	void do_read();

	void do_write();

	void clean_conn();

	void do_connect();

	void set_msg_callback(msg_callback* msg_cb)
	{
		this->_msg_callback = msg_cb;		
	}

	input_buf ibuf;
	output_buf obuf;

	struct sockaddr_in _server_addr;
	socklen_t _addrlen;

private:
	int _sockfd;

	event_loop* _loop;

	msg_callback* _msg_callback;
};
