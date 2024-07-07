#include <sys/epoll.h>
#include "event_loop.h"


event_loop::event_loop()
{
	_epfd = epoll_create1(0);	
	if (_epfd == -1)
	{
		fprintf(stderr, "epoll create error\n");		
		exit(1);
	}

	_ready_tasks.clear();
}

/* 阻塞循环监听, 并且处理epoll_wait, 包含调用对应的触发回调函数 */
void event_loop::event_process()
{
	io_event_map_it ev_it;
	while (true)
	{
		int nfds = epoll_wait(_epfd, _fired_evs, MAXEVENTS, -1);
		for (int i = 0; i < nfds; ++i)
		{
			ev_it = _io_evs.find(_fired_evs[i].data.fd);		

			io_event* ev = &(ev_it->second);

			if (_fired_evs[i].events & EPOLLIN)
			{
				void* args = ev->read_callback_args;		
				ev->read_callback(this, _fired_evs[i].data.fd, args);
			}
			else if (_fired_evs[i].events & EPOLLOUT)
			{
				void* args = ev->write_callback_args;		
				ev->write_callback(this, _fired_evs[i].data.fd, args);
			}
			else if (_fired_evs[i].events & (EPOLLHUP | EPOLLERR))
			{
				// 水平触发未处理, 可能会出现HUP事件, 需要正常处理读写, 如果当前时间events既没有写, 也没有读, 将events从epoll中删除		
				if (ev->read_callback != NULL)
				{
					// 读事件, 调用读回调函数		
					void* args = ev->read_callback_args;
					ev->read_callback(this, _fired_evs[i].data.fd, args);
				}
				else if (ev->write_callback != NULL)
				{
					void* args = ev->write_callback_args;
					ev->write_callback(this, _fired_evs[i].data.fd, args);
				}
				else
				{
					// 删除该事件		
					fprintf(stderr, "fd %d get error, delete from epoll_loop\n", _fired_evs[i].data.fd);
					this->del_io_event(_fired_evs[i].data.fd);
				}
			}
		}

		// 每次全部的fd的读事件执行完
		// 处理其他的task任务
		this->execute_ready_tasks();
	}
}

void event_loop::add_io_event(int fd, io_callback* proc, int mask, void* args)
{
	int final_mask;	
	int op;

	// 1 查看当前fd是否是已有事件
	io_event_map_it it = _io_evs.find(fd);
	if (it == _io_evs.end())
	{
		op = EPOLL_CTL_ADD;		
		final_mask = mask;
	}
	else
	{
		op = EPOLL_CTL_MOD;		
		final_mask = it->second.mask | mask;
	}

	// 2 将fd 和io_callback 绑定到map 中
	if (mask & EPOLLIN)
	{
		_io_evs[fd].read_callback = proc;		
		_io_evs[fd].read_callback_args = args;
	}
	else if (mask & EPOLLOUT)
	{
		_io_evs[fd].write_callback = proc;		
		_io_evs[fd].write_callback_args = args;
	}

	_io_evs[fd].mask = final_mask;

	struct epoll_event event;
	event.events = final_mask;
	event.data.fd = fd;
	if (epoll_ctl(_epfd, op, fd, &event) == -1)
	{
		fprintf(stderr, "epoll ctl %d error\n", fd);		
		return;
	}

	listen_fds.insert(fd);
}

void event_loop::del_io_event(int fd)
{
	io_event_map_it it = _io_evs.find(fd);
	if (it == _io_evs.end())
	{
		return;		
	}

	_io_evs.erase(fd);

	listen_fds.erase(fd);

	epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL);
}

void event_loop::del_io_event(int fd, int mask)
{
	io_event_map_it it = _io_evs.find(fd);
	if (it == _io_evs.end())
	{
		return;		
	}

	int& o_mask = it->second.mask;
	o_mask = o_mask & (~mask);

	if (o_mask == 0)
	{
		this->del_io_event(fd);		
	}
	else
	{
		struct epoll_event event;		
		event.events = o_mask;
		event.data.fd = fd;

		epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &event);
	}
}

// ==================== 对于 异步task任务的方法 =========================
/* 添加一个task任务到_ready_tasks集合中 */
void event_loop::add_task(task_func func, void* args)
{
	task_func_pair func_pair(func, args);
	_ready_tasks.push_back(func_pair);
}

void event_loop::execute_ready_tasks()
{
	std::vector<task_func_pair>::iterator it;

	for (it = _ready_tasks.begin(); it != _ready_tasks.end(); it++)
	{
		task_func func = it->first;		
		void* args = it->second;

		func(this, args);
	}

	_ready_tasks.clear();
}
