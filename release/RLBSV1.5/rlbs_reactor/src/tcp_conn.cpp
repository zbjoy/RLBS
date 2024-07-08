#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "message.h"
#include "tcp_server.h"
#include "tcp_conn.h"


void conn_rd_callback(event_loop* loop, int fd, void* args)
{
	tcp_conn* conn = (tcp_conn*)args;	
	conn->do_read();
}

void conn_wt_callback(event_loop* loop, int fd, void* args)
{
	tcp_conn* conn = (tcp_conn*)args;	
	conn->do_write();
}

tcp_conn::tcp_conn(int connfd, event_loop* loop)
{
	_connfd = connfd;
	_loop = loop;

	int flag = fcntl(_connfd, F_SETFL, 0);
	fcntl(_connfd, F_SETFL, O_NONBLOCK | flag);

	// 2 设置TCP_NODELAY状态, 禁止读写缓存, 降低小包延迟
	int op = 1;
	setsockopt(_connfd, IPPROTO_TCP, TCP_NODELAY, &op, sizeof(op));

	// 3 要触发的Hook函数
	if (tcp_server::conn_start_cb != NULL)
	{
		tcp_server::conn_start_cb(this, tcp_server::conn_start_cb_args);		
	}

	_loop->add_io_event(_connfd, conn_rd_callback, EPOLLIN, this);

	tcp_server::increase_conn(_connfd, this);
}

void tcp_conn::do_read()
{
	int ret = ibuf.read_data(_connfd);	
	if (ret == -1)
	{
		fprintf(stderr, "(tcp_conn::do_raed() error)read data from socket\n");		
		this->clean_conn();
		return;
	}
	else if (ret == 0)
	{
		printf("peer client closed!!\n");		
		this->clean_conn();
	}

	msg_head head;

	while (ibuf.length() >= MESSAGE_HEAD_LEN)
	{
		memcpy(&head, ibuf.data(), MESSAGE_HEAD_LEN);		
		if (head.msglen > MESSAGE_LENGTH_LIMIT || head.msglen < 0)
		{
			fprintf(stderr, "(tcp_conn::do_read() error)data format error, too large or too small, need close\n");		
			this->clean_conn();
			break;
		}

		if (ibuf.length() < MESSAGE_HEAD_LEN + head.msglen)
		{
			break;		
		}

		ibuf.pop(MESSAGE_HEAD_LEN);

		tcp_server::router.call(head.msgid, head.msglen, ibuf.data(), this);

		ibuf.pop(head.msglen);
	}

	ibuf.adjust();

	return;
}

void tcp_conn::do_write()
{
	while (obuf.length())	
	{
		int write_num = obuf.write2fd(_connfd);		
		if (write_num == -1)
		{
			fprintf(stderr, "tcp_conn write connfd error\n");		
			this->clean_conn();
			return;
		}
		else if (write_num == 0)
		{
			break;		
		}
	}

	if (obuf.length() == 0)
	{
		_loop->del_io_event(_connfd, EPOLLOUT);		
	}

	return;
}

int tcp_conn::send_message(const char* data, int msglen, int msgid)
{
	bool active_epollout = false;

	if (obuf.length() == 0)
	{
		active_epollout = true;		
	}

	msg_head head;
	head.msgid = msgid;
	head.msglen = msglen;

	int ret = obuf.send_data((const char*)&head, MESSAGE_HEAD_LEN);
	if (ret != 0)
	{
		fprintf(stderr, "send head error\n");		
		return -1;
	}

	ret = obuf.send_data(data, msglen);
	if (ret != 0)
	{
		fprintf(stderr, "send data error\n");		
		obuf.pop(MESSAGE_HEAD_LEN);
		return -1;
	}

	if (active_epollout == true)
	{
		_loop->add_io_event(_connfd, conn_wt_callback, EPOLLOUT, this);		
	}

	return 0;
}

void tcp_conn::clean_conn()
{
	if (tcp_server::conn_close_cb != NULL)	
	{
		tcp_server::conn_close_cb(this, tcp_server::conn_close_cb_args);		
	}

	tcp_server::decrease_conn(_connfd);

	_loop->del_io_event(_connfd);

	ibuf.clear();
	obuf.clear();

	close(_connfd);
}
