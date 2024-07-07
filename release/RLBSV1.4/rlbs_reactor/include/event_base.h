#pragma once

class event_loop;


// IO事件触发的回调函数
typedef void io_callback(event_loop* loop, int fd, void* args);

struct io_event
{
	io_event()
	{
		mask = 0;		
		write_callback = NULL;
		read_callback = NULL;
		read_callback_args = NULL;
		write_callback_args = NULL;
	}	

	// 事件的读写属性
	int mask; // eg: EPOLLIN EPOLLOUT

	io_callback* read_callback;
	io_callback* write_callback;

	void* read_callback_args;
	void* write_callback_args;
};
