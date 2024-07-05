#pragma once

#include "event_loop.h"

struct task_msg
{
	enum TASK_TYPE
	{
		NEW_CONN, // 新建连接的任务
		NEW_TASK  // 一般的任务
	};

	TASK_TYPE type;

	union
	{
		// 任务1 NEW_CONN
		int connfd;		

		struct
		{
			void (*task_cb)(event_loop* loop, void* args);		
			void* args;
		};
	};
};
