#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "message.h"
#include "reactor_buf.h"
#include "tcp_client.h"

void read_callback(event_loop* loop, int fd, void* args)
{
	tcp_client* cli = (tcp_client*)args;	
	cli->do_read();
}

void write_callback(event_loop* loop, int fd, void* args)
{
	tcp_client* cli = (tcp_client*)args;	
	cli->do_write();
}

tcp_client::tcp_client(event_loop* loop, const char* ip, unsigned short port)
{
	_sockfd = -1;	
	// _msg_callback = NULL;
	_loop = loop;
	_conn_start_cb = NULL;
	_conn_close_cb = NULL;
	_conn_start_cb_args = NULL;
	_conn_close_cb_args = NULL;

	bzero(&_server_addr, sizeof(_server_addr));
	_server_addr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &_server_addr.sin_addr.s_addr);
	_server_addr.sin_port = htons(port);
	_addrlen = sizeof(_server_addr);

	this->do_connect();
}

int tcp_client::send_message(const char* data, int msglen, int msgid)
{
	// printf("tcp_client::send_message()...\n");	

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
		/*  --------------------------------------------------- */
		// 			obuf.pop(MESSAGE_HEAD_LEN);
		/*  --------------------------------------------------- */
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
		// printf("send_message active epoll out!!\n");		
		_loop->add_io_event(_sockfd, write_callback, EPOLLOUT, this);
	}
	return 0;
}

void tcp_client::do_read()
{
	// printf("tcp_client::da_read()...\n");	

	int ret = ibuf.read_data(_sockfd);

#if 1
	printf("void tcp_client::do_read()\n");
#endif

	if (ret == -1)
	{
		fprintf(stderr, "(error)read data from socket\n");
		this->clean_conn();
		return;
	}
	else if (ret == 0)
	{
		printf("peer server closed!!\n");		
		this->clean_conn();
	}

	msg_head head;

	while (ibuf.length() >= MESSAGE_HEAD_LEN)
	{
		memcpy(&head, ibuf.data(), MESSAGE_HEAD_LEN);		
		if (head.msglen > MESSAGE_LENGTH_LIMIT || head.msglen < 0)
		{
			fprintf(stderr, "data format error, too large or too small, need close\n");		
			this->clean_conn();
			break;
		}

		if (ibuf.length() < MESSAGE_HEAD_LEN + head.msglen)
		{
			break;		
		}

		ibuf.pop(MESSAGE_HEAD_LEN);

#if 0
		if (_msg_callback != NULL)
		{
			this->_msg_callback(ibuf.data(), head.msglen, head.msgid, this, NULL);
		}
#endif

		// 调用注册的回调业务
#if 1
		printf("head.msgid: %d\n", head.msgid);
		printf("head.msglen: %d\n", head.msglen);
#endif
		this->_router.call(head.msgid, head.msglen, ibuf.data(), this);

		// 整个消息都处理完了
		ibuf.pop(head.msglen);
	}
	ibuf.adjust();
	return;
}

//处理业务
void tcp_client::do_write()
{
	// printf("tcp_client::do_write()...\n");

	while (obuf.length()) 
	{
		int write_num = obuf.write2fd(_sockfd);
		if(write_num == -1)
		{
			fprintf(stderr,"tcp_client write fd error\n");
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
		_loop->del_io_event(_sockfd, EPOLLOUT);		
	}

	return;
}

// 释放链接
void tcp_client::clean_conn()
{
	if (_conn_close_cb != NULL)
	{
		_conn_close_cb(this, _conn_close_cb_args);		
	}

	if (_sockfd != -1)
	{
		printf("clean conn, del socket\n");		
		_loop->del_io_event(_sockfd);
		close(_sockfd);
	}

	// 重新发起链接
	// this->do_connect();
}

void connection_succ(event_loop* loop, int fd, void* args)
{
	tcp_client* cli = (tcp_client*)args;	
	loop->del_io_event(fd);

	int result = 0;
	socklen_t result_len = sizeof(result);
	getsockopt(fd, SOL_SOCKET, SO_ERROR, &result, &result_len);
	if (result == 0)
	{
#if 0
		printf("connection success!!\n");		

		const char* msg = "Hello RLBS, i am client!!\n";
		int msgid = 1;

		cli->send_message(msg, strlen(msg), msgid);
#endif

		if (cli->_conn_start_cb != NULL)
		{
			cli->_conn_start_cb(cli, cli->_conn_start_cb_args);		
		}

		loop->add_io_event(fd, read_callback, EPOLLIN,cli);

		if (cli->obuf.length() != 0)
		{
			loop->add_io_event(fd, write_callback, EPOLLOUT, cli);		
		}
	}
	else
	{
		fprintf(stderr, "connection %s:%d error\n", inet_ntoa(cli->_server_addr.sin_addr), ntohs(cli->_server_addr.sin_port));		
		return;
	}
}

void tcp_client::do_connect()
{
	if (_sockfd != -1)	
	{
		close(_sockfd);		
	}

	_sockfd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, IPPROTO_TCP);
	if (_sockfd == -1)
	{
		fprintf(stderr, "create tcp client socket error\n");		
		exit(1);
	}

	int ret = connect(_sockfd, (const struct sockaddr*)&_server_addr, _addrlen);
	if (ret == 0)
	{
		printf("connect ret == 0, connect %s:%d success!!\n", inet_ntoa(_server_addr.sin_addr), ntohs(_server_addr.sin_port));		

		connection_succ(_loop, _sockfd, this);
	}
	else
	{
		if (errno == EINPROGRESS)
		{
			printf("do_connect, EINPROGRESS\n");		

			_loop->add_io_event(_sockfd, connection_succ, EPOLLOUT, this);
		}
		else
		{
			fprintf(stderr, "connection error\n");		
			exit(1);
		}
	}
}
