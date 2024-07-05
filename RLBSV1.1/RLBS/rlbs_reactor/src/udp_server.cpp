#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include "udp_server.h"

void read_callback(event_loop* loop, int fd, void* args)
{
	udp_server* server = (udp_server*)args;	

	server->do_read();
}

udp_server::udp_server(event_loop* loop, const char* ip, uint16_t port)
{
	if (signal(SIGHUP, SIG_IGN) == SIG_ERR)	
	{
		fprintf(stderr, "signal ignore SIGHUP\n");		
	}

	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
	{
		fprintf(stderr, "signal ignore SIGPIPE\n");		
	}

	_sockfd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_UDP);
	if (_sockfd == -1)
	{
		perror("create udp server");		
		exit(1);
	}

	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &server_addr.sin_addr.s_addr);
	server_addr.sin_port = htons(port);

	if (bind(_sockfd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		fprintf(stderr, "bind error\n");		
		exit(1);
	}

	this->_loop = loop;

	bzero(&_client_addr, sizeof(_client_addr));
	_client_addrlen = sizeof(_client_addr);

	_loop->add_io_event(_sockfd, read_callback, EPOLLIN, this);

	printf("udp server is running ip = %s, port = %d\n", ip, port);
}

void udp_server::do_read()
{
	while (true)	
	{
		int pkg_len = recvfrom(_sockfd, _read_buf, sizeof(_read_buf), 0, (struct sockaddr*)&_client_addr, &_client_addrlen);		
		if (pkg_len == -1)
		{
			if (errno == EINTR)		
			{
				// 中断错误			
				continue;
			}
			else if (errno == EAGAIN)
			{
				// 非阻塞, 不是错误		
				break;
			}
			else
			{
				perror("udp server recvfrom error");		
				break;
			}
		}
		
		msg_head head;
		memcpy(&head, _read_buf, MESSAGE_HEAD_LEN);

		if (head.msglen > MESSAGE_LENGTH_LIMIT || head.msglen < 0 || head.msglen + MESSAGE_HEAD_LEN != pkg_len)
		{
			fprintf(stderr, "error message head format error\n");		
			continue;
		}

		_router.call(head.msgid, head.msglen, _read_buf + MESSAGE_HEAD_LEN, this);
	}
}

int udp_server::send_message(const char* data, int msglen, int msgid)
{
	if (msglen > MESSAGE_LENGTH_LIMIT)	
	{
		fprintf(stderr, "too big message\n");		
		return -1;
	}

	msg_head head;
	head.msglen = msglen;
	head.msgid = msgid;

	memcpy(_write_buf, &head, MESSAGE_HEAD_LEN);
	memcpy(_write_buf + MESSAGE_HEAD_LEN, data, msglen);

	int ret = sendto(_sockfd, _write_buf, msglen + MESSAGE_HEAD_LEN, 0, (struct sockaddr*)&_client_addr, _client_addrlen);
	if (ret == -1)
	{
		perror("sendto()");
		return -1;
	}

	return ret;
}

void udp_server::add_msg_router(int msgid, msg_callback* cb, void* user_data)
{
	_router.register_msg_router(msgid, cb, user_data);	
}

udp_server::~udp_server()
{
	_loop->del_io_event(_sockfd);	
	close(_sockfd);
}
