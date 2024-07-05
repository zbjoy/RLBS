#pragma once

class net_connection
{
public:	
	net_connection() {}

	// 纯虚函数
	virtual int send_message(const char* data, int msglen, int msgid) = 0;

	virtual int get_fd() = 0;

	// 开发者可以通过该参数传递一些动态的自定义参数
	void* param;
};

typedef void (*conn_callback)(net_connection* conn, void* args);
