#pragma once

#include <netinet/in.h>
#include "event_loop.h"
#include "tcp_conn.h"
#include "message.h"

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

public:
	static tcp_conn** conns;
	static void increase_conn(int coonfd, tcp_conn* conn);
	static void decrease_conn(int connfd);
	static void get_conn_num(int* curr_conn);

	static pthread_mutex_t _conns_mutex;

#define MAX_CONNS 3
	static int _max_conns;
	static int _curr_conns;

	// 添加路由分发机制
	static msg_router router;

	void add_msg_router(int msgid, msg_callback* cb, void* user_data = NULL)
	{
		router.register_msg_router(msgid, cb, user_data);
	}
};
