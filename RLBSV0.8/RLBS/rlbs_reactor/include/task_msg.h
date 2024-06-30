#pragma once

/*
 *
 * thread_queue消息对列 所能够接受的消息类型
 *
 * */

struct task_msg
{
	// 两类
	// 1. 新建立连接的任务
	// 2. 一般的普通任务  // eg: 主 thread 希望分发一些任务给每个线程来处理
	enum TASK_TYPE
	{
		NEW_CONN, // 新建连接的任务
		NEW_TASK  // 一般的任务
	};

	TASK_TYPE type; // 任务类型


	// 任务本身的数据内容
	union {
		// 任务1		
		int connfd;

		//TODO
		// 任务2, task_msg的任务内容应该由具体的数据参数和具体的回调业务
		struct {
			void (*task_cb)(event_loop* loop, void* args);		
			void* args;
		};
	};
};
