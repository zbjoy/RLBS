#include <iostream>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "rlbs_reactor.h"
#include "tcp_server.h"
#include "reactor_buf.h"
#include "task_msg.h"
#include "config_file.h"

// ================= 针对连接管理的初始化 =======================
tcp_conn* *tcp_server::conns = NULL;
pthread_mutex_t tcp_server::_conns_mutex = PTHREAD_MUTEX_INITIALIZER;
int tcp_server::_curr_conns = 0;
int tcp_server::_max_conns = 0;

// ================== 初始化 路由分发机制句柄 ====================
msg_router tcp_server::router;

// ================== 初始化 Hook 函数 =====================
conn_callback tcp_server::conn_start_cb = NULL;
conn_callback tcp_server::conn_close_cb = NULL;
void* tcp_server::conn_start_cb_args = NULL;
void* tcp_server::conn_close_cb_args = NULL;


#if 0
	void server_rd_callback(event_loop* loop, int fd, void* args);
	void server_wt_callback(event_loop* loop, int fd, void* args);
#endif

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

void rlbs_hello()
{
	std::cout << "RLBS hello......" << std::endl;	
}

#if 0
	// 临时的收发消息结构
	struct message
	{
		char data[m4K];
		char len;
	};

	struct message msg;
#endif

void accept_callback(event_loop* loop, int fd, void* args)
{
	tcp_server* server = (tcp_server*)args;	
	server->do_accept();
}

tcp_server::tcp_server(event_loop* loop, const char* ip, uint16_t port)
{	
	_thread_pool = NULL;

	if (signal(SIGHUP, SIG_IGN))
	{
		fprintf(stderr, "signal ignore SIGHUP\n");			
	}

	if (signal(SIGPIPE, SIG_IGN))
	{
		fprintf(stderr, "signal ignore SIGPIPE");		
	}

	_sockfd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
	if (_sockfd == -1)
	{
		fprintf(stderr, "tcp::server: socket()\n");		
		exit(1);
	}

	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	// inet_aton
	inet_pton(AF_INET, ip, &server_addr.sin_addr.s_addr);
	server_addr.sin_port = htons(port);

	int opt = 1;
	if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
	{
		fprintf(stderr, "setsockopt reuse error\n");		
	}


	if (bind(_sockfd, (const struct sockaddr*)&server_addr, sizeof(server_addr)))
	{
		fprintf(stderr, "bind error\n");		
		exit(1);
	}

	if (listen(_sockfd, 500) == -1)
	{
		fprintf(stderr, "listen error\n");		
		exit(1);
	}

	// 将形参loop添加到tcp_server _loop中
	_loop = loop;

	// 创建线程池
	// int thread_cnt = 2; // TODO 从配置文件中读取(一般与cpu个数相同)
	int thread_cnt = config_file::instance()->GetNumber("reactor", "threadNum", 3);
	
	if (thread_cnt > 0)
	{
		_thread_pool = new thread_pool(thread_cnt);		
		if (_thread_pool == NULL)
		{
			fprintf(stderr, "tcp server new thread_pool error\n");		
			exit(1);
		}
	}


	// 创建连接管理
	// _max_conns = MAX_CONNS; // TODO 从配置文件中读最大个数
	_max_conns = config_file::instance()->GetNumber("reactor", "maxConn", 1000);
	conns = new tcp_conn*[_max_conns + 5 + 2 * thread_cnt];
	if (conns == NULL)
	{
		fprintf(stderr, "new conns[%d] error\n", _max_conns);		
		exit(1);
	}

	// 注册_sockfd读事件--->acppet处理
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
				fprintf(stderr, "accept error is EINTR");
				continue;
			}
			else if (errno == EAGAIN)
			{
				fprintf(stderr, "accept error is EAGAIN");
				continue;
				
			}
			else if (errno == EMFILE)
			{
				fprintf(stderr, "accept error is EMFILE");
				continue;
			}
			else
			{
				fprintf(stderr, "accept error is ");
				exit(1);
			}
		}
		else
		{
			//TODO 添加心跳机制
			//TODO 添加消息队列


			// printf("accept success!!\n");

			// this->_loop->add_io_event(connfd, server_rd_callback, EPOLLIN, &msg);
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
					// 开启了线程池
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
						fprintf(stderr, "new tcp_conn error");		
						exit(1);
					}
					printf("get new connection success!!(no thread_pool)\n");
					
				}
			}

			break;

		}
	}
}


tcp_server::~tcp_server()
{
	close(_sockfd);
}

#if 0
	// 客户端connfd注册的写事件回调业务
	void server_wt_callback(event_loop* loop, int fd, void* args)
	{
		struct message* msg = (struct message*)args;
		output_buf obuf;
	
		obuf.send_data(msg->data, msg->len);
		while (obuf.length())
		{
			int write_num = obuf.write2fd(fd);		
			if (write_num == -1)
			{
				fprintf(stderr, "write connfd error\n");		
				return;
			}
			else if (write_num == 0)
			{
				break;		
			}
		}
	
		    //删除写事件， 添加读事件
			loop->del_io_event(fd, EPOLLOUT);
			loop->add_io_event(fd, server_rd_callback, EPOLLIN, msg);
	}
	
	// 客户端connfd注册的读事件的回调业务
	void server_rd_callback(event_loop* loop, int fd, void* args)
	{
		struct message* msg = (struct message*)args;
		int ret = 0;
		input_buf ibuf;
	
		printf("begin read...\n");
		ret = ibuf.read_data(fd);
	
		if (ret == 0)
		{
			fprintf(stderr, "ibuf read_data error\n");		
	
			loop->del_io_event(fd);
	
			close(fd);
			return;
		}
	
		printf("server rd callback is called!\n");
	
	
		//将读到的数据拷贝到msg中
		msg->len = ibuf.length();
		bzero(msg->data, msg->len);
		memcpy(msg->data, ibuf.data(), msg->len);
	
		ibuf.pop(msg->len);
		ibuf.adjust();
	
	
		printf(" recv data = %s\n", msg->data);
	
	
		//针对connfd  删除读事件， 添加写事件
		loop->del_io_event(fd, EPOLLIN);
		loop->add_io_event(fd, server_wt_callback, EPOLLOUT, msg);//epoll_wait会立刻触发EPOLLOUT事件
	
	}

#endif
