#pragma once

#include <netinet/in.h>
#include "event_loop.h"
#include "tcp_conn.h"
#include "message.h"
#include "thread_pool.h"

class tcp_server
{
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
	static void increase_conn(int connfd, tcp_conn* conn);
	static void decrease_conn(int connfd);
	static void get_conn_num(int* curr_conn);

	static pthread_mutex_t _conns_mutex;

#define MAX_CONNS 3 //TODO 从配置文件中读取
	static int _max_conns;
	static int _curr_conns;

	static msg_router router;

	void add_msg_router(int msgid, msg_callback* cb, void* user_data = NULL)
	{
		router.register_msg_router(msgid, cb, user_data);		
	}

	/* 连接创建后的Hook函数 */
	static void set_conn_start(conn_callback cb, void* args = NULL)
	{
		conn_start_cb = cb;		
		conn_start_cb_args = args;
	}

	/* 连接销毁之前的Hook函数 */
	static void set_conn_close(conn_callback cb, void* args = NULL)
	{
		conn_close_cb = cb;		
		conn_close_cb_args = args;
	}

	static conn_callback conn_start_cb;
	static void* conn_start_cb_args;

	static conn_callback conn_close_cb;
	static void* conn_close_cb_args;

	thread_pool* get_th_pool()
	{
		return _thread_pool;		
	}

private:
	// ===================== 连接池 =========================
	thread_pool* _thread_pool;

};
