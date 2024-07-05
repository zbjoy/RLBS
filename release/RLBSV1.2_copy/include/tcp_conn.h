#pragma once

#include "event_loop.h"
#include "reactor_buf.h"
#include "net_connection.h"

class tcp_conn : public net_connection
{
public:
	tcp_conn(int connfd, event_loop* loop);

	void do_read();

	void do_write();

	virtual int send_message(const char* data, int msglen, int msgid);

	void clean_conn();

private:
	int _connfd;

	event_loop* _loop;

	output_buf obuf;

	input_buf ibuf;

};
