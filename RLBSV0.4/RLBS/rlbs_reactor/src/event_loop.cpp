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
}

// 阻塞循环监听事件
void event_loop::event_process()
{
	io_event_map_it ev_it;	
	while (true)
	{
		printf("wait IN OUT event...\n");		
		for (listen_fd_set::iterator it = listen_fds.begin(); it != listen_fds.end(); it++)
		{
			printf("fd %d is listening by event_loop...\n", *it);		
		}

		int nfds = epoll_wait(_epfd, _fired_evs, MAXEVENTS, -1);

		for (int i = 0; i < nfds; i++)
		{
			ev_it = _io_evs.find(_fired_evs[i].data.fd);
			
			io_event* ev = &(ev_it->second);

			if (_fired_evs[i].events & EPOLLIN)
			{
				void* args = ev->rcb_args;		
				// 调用业务
				ev->read_callback(this, _fired_evs[i].data.fd, args);
			}
			else if (_fired_evs[i].events & EPOLLOUT)
			{
				void* args = ev->wcb_args;
				// 调用业务
				ev->write_callback(this, _fired_evs[i].data.fd, args);
			}
			else if (_fired_evs[i].events & (EPOLLHUP | EPOLLERR))
			{
				// 水平触发未处理, 可能会出现HUP事件, 需要正常处理读写, 如果当前事件events既没有写, 也没有读, 需要将events从epoll中删除
				if (ev->read_callback != NULL)
				{
					void* args = ev->rcb_args;		
					ev->read_callback(this, _fired_evs[i].data.fd, args);
				}
				else if (ev->write_callback != NULL)
				{
					void* args = ev->wcb_args;		
					ev->write_callback(this, _fired_evs[i].data.fd, args);
				}
				else
				{
					// 删除		
					fprintf(stderr, "fd %d get error, delete from epoll", _fired_evs[i].data.fd);
					this->del_io_event(_fired_evs[i].data.fd);
				}
			}
		}
	}
}

void event_loop::add_io_event(int fd, io_callback* proc, int mask, void* args)
{
	int final_mask;	
	int op;

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

	if (mask & EPOLLIN)
	{
		_io_evs[fd].read_callback = proc;		
		_io_evs[fd].rcb_args = args;
	}
	else if (mask & EPOLLOUT)
	{
		_io_evs[fd].write_callback = proc;	
		_io_evs[fd].wcb_args = args;
	}

	// 将当前事件添加到原生的epoll中
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

// 从event_loop中删除一个io事件
void event_loop::del_io_event(int fd)
{
	io_event_map_it it = _io_evs.find(fd);	
	if (it == _io_evs.end())
	{
		return;		
	}

	// 将事件从_io_evs(map)中删除
	_io_evs.erase(fd);

	// 将事件从listen_fds(set)中删除
	listen_fds.erase(fd);

	// 将事件从原生epoll中删除
	epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL);
}

void event_loop::del_io_event(int fd, int mask)
{
	io_event_map_it it = _io_evs.find(fd);	
	if (it == _io_evs.end())
	{
		return;		
	}

	int o_mask = it->second.mask;
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
