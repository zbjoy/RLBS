#pragma once

#include <stdio.h>


class event_loop;

// IO事件触发回调函数
typedef void io_callback(event_loop* loop, int fd, void* args);

/*
 * 封装一次IO触发的机制
 *
 * */

struct io_event
{
	io_event()
	{
		mask = 0;		
		write_callback = NULL;
		read_callback = NULL;
		rcb_args = NULL;
		wcb_args = NULL;
	}

	// 事件的读写属性
	int mask; // 一般只考虑 EPOLLIN, EPOLLOUT
	
	// 读事件触发所绑定的回调函数
	io_callback* read_callback;
	
	// 写事件触发所绑定的回调函数
	io_callback* write_callback;

	// 读事件回调函数的形参
	void* rcb_args;
	
	// 写事件回调函数的形参
	void* wcb_args;
};
