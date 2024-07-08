#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <strings.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "tcp_server.h"
#include "tcp_conn.h"
#include "reactor_buf.h"
#include "config_file.h"


/* ========================== 针对连接管理的初始化 ============================= */
tcp_conn** tcp_server::conns = NULL;
pthread_mutex_t tcp_server::_conns_mutex = PTHREAD_MUTEX_INITIALIZER;
int tcp_server::_max_conns = 0;
int tcp_server::_curr_conns = 0;



/* ======================= 初始化路由分发机制 =============================== */
msg_router tcp_server::router;

/* ======================== 初始化Hook函数 ========================= */
conn_callback tcp_server::conn_start_cb = NULL;
void* tcp_server::conn_start_cb_args = NULL;
conn_callback tcp_server::conn_close_cb = NULL;
void* tcp_server::conn_close_cb_args = NULL;



void tcp_server::increase_conn(int connfd, tcp_conn* conn)
{
	pthread_mutex_lock(&_conns_mutex);
	conns[connfd] = conn;
	_curr_conns++;
	pthread_mutex_unlock(&_conns_mutex);
}


void tcp_server::decrease_conn(int connfd)
{
	pthread_mutex_lock(&_conns_mutex);	
	conns[connfd] = NULL;
	_curr_conns--;
	pthread_mutex_unlock(&_conns_mutex);
}

void tcp_server::get_conn_num(int* curr_conn)
{
	pthread_mutex_lock(&_conns_mutex);
	*curr_conn = _curr_conns;
	pthread_mutex_unlock(&_conns_mutex);
}

void accept_callback(event_loop* loop, int fd, void* args)
{
	tcp_server* server = (tcp_server*)args;	
	server->do_accept();
}

tcp_server::tcp_server(event_loop* loop, const char* ip, uint16_t port)
{
	_thread_pool = NULL;

	if (signal(SIGHUP, SIG_IGN) == SIG_ERR)
	{
		fprintf(stderr, "signal ignore SIGHUP\n");		
	}

	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
	{
		fprintf(stderr, "signal ignore SIGPIPE\n");		
	}

	_sockfd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
	if (_sockfd == -1)
	{
		fprintf(stderr, "(tcp_server:: socket() error)\n");		
		exit(1);
	}

	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &server_addr.sin_addr.s_addr);
	/* ------------------------------------------------------ */
	// 少写了一个这导致改了3个小时bug(总共两天下来)
	server_addr.sin_port = htons(port);
	/* ------------------------------------------------------ */

	int op = 1;
	if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) < 0)
	{
		fprintf(stderr, "(tcp_server::setsockopt() error\n)");		
	}

	if (bind(_sockfd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		fprintf(stderr, "bind error\n");		
		exit(1);
	}

	if (listen(_sockfd, 500) == -1)
	{
		fprintf(stderr, "listen error\n");		
		exit(1);
	}

	_loop = loop;

	int thread_cnt = config_file::instance()->GetNumber("reactor", "threadNum", 3);
	if (thread_cnt > 0)
	{
		_thread_pool = new thread_pool(thread_cnt);		
		if (_thread_pool == NULL)
		{
			fprintf(stderr, "tcp_server new thread_pool error\n");		
			exit(1);
		}
	}

	_max_conns = config_file::instance()->GetNumber("reactor", "maxConn", 1000);

	// 这个不是很可取, 因为可能开发者会使用一些fd导致在太大时有一定的误差
	conns = new tcp_conn*[_max_conns + 5 + 2 * thread_cnt];

	if (conns == NULL)
	{
		fprintf(stderr, "new conns[%d] error\n", _max_conns);		
		exit(1);
	}

	_loop->add_io_event(_sockfd, accept_callback, EPOLLIN, this);
}

void tcp_server::do_accept()
{
	int connfd;
	while (true)
	{
		_addrlen = 0;
		connfd = accept(_sockfd, (struct sockaddr*)&_connaddr, &_addrlen);	
		if (connfd == -1)
		{
			if (errno == EINTR)
			{
				fprintf(stderr, "accept errno == EINTR\n");		
				continue;
			}
			else if (errno == EAGAIN)
			{
				fprintf(stderr, "accept errno == EAGAIN\n");		
				break;
			}
			else if (errno == EMFILE)
			{
				fprintf(stderr, "accept errno == EMFILE\n");		
				continue;
			}
			else
			{
				fprintf(stderr, "accept error\n");		
				perror("accept:");
				exit(1);
			}

		}
		else
		{
			int cur_conns;
			get_conn_num(&cur_conns);
			if (cur_conns >= _max_conns)
			{
				fprintf(stderr, "so many connections, max = %d\n", _max_conns);		
				close(connfd);
			}
			else
			{
				if (_thread_pool != NULL)		
				{
					thread_queue<task_msg>* queue = _thread_pool->get_thread();		

					task_msg task;
					task.type = task_msg::NEW_CONN;
					task.connfd = connfd;

					queue->send(task);
				}
				else
				{
					tcp_conn* conn = new tcp_conn(connfd, _loop);		
					if (conn == NULL)
					{
						fprintf(stderr, "new tcp_conn error\n");		
						exit(1);
					}
					break;
				}
			}
		}


	}

}

tcp_server::~tcp_server()
{
	close(_sockfd);	
}




