#pragma once

#include "event_loop.h"
#include "reactor_buf.h"

class tcp_conn
{
public:
	tcp_conn(int connfd, event_loop* loop);

	void do_read();

	void do_write();

	int send_message(const char* data, int msglen, int msgid);

	void clean_conn();

private:

	int _connfd;

	event_loop* _loop;

	output_buf obuf;

	input_buf ibuf;
};
