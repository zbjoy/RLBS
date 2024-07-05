#pragma once

#include <iostream>
#include <ext/hash_map>
#include "net_connection.h"

struct msg_head
{
	int msgid;
	int msglen;
};

#define MESSAGE_HEAD_LEN 8

#define MESSAGE_LENGTH_LIMIT (65535 - MESSAGE_HEAD_LEN)

// 定义一个路由回调函数的数据类型
typedef void msg_callback(const char* data, uint32_t len, int msgid, net_connection* conn, void* user_data);

class msg_router
{
public:	
	msg_router(): _router(), _args()
	{
		printf("msg_router init...\n");
	}

	// 给一个消息ID注册一个对应的回调业务函数
	int register_msg_router(int msgid, msg_callback* msg_cb, void* user_data)
	{
		if (_router.find(msgid) != _router.end())		
		{
			// std::cout << "msgID: " << msgid << " is already register..." << std::endl;		
			return -1;
		}

		std::cout << " add msg callback msgid(register) = " << msgid << std::endl;

		_router[msgid] = msg_cb;
		_args[msgid] = user_data;
		
		return 0;
	}

	void call(int msgid, uint32_t msglen, const char* data, net_connection* conn)
	{
		// std::cout << "call msgid = " << msgid << std::endl;

		if (_router.find(msgid) == _router.end())
		{
			// std::cout << "msgid " << msgid << " is not register" << std::endl;		
			return;
		}

		msg_callback* callback = _router[msgid];
		void* user_data = _args[msgid];

		callback(data, msglen, msgid, conn, user_data);
		// printf("============================");
	}

private:
	__gnu_cxx::hash_map<int, msg_callback*> _router;

	__gnu_cxx::hash_map<int, void*> _args;

};

