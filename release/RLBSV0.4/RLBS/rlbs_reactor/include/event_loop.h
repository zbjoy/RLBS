#pragma once
#include "event_base.h"
#include <ext/hash_map>
#include <ext/hash_set>
#include <sys/epoll.h>

#define MAXEVENTS 10

typedef __gnu_cxx::hash_map<int, io_event> io_event_map;
typedef __gnu_cxx::hash_map<int, io_event>::iterator io_event_map_it;

typedef __gnu_cxx::hash_set<int> listen_fd_set;


class event_loop
{
public:
	// 构造, 用于创建epoll
	event_loop();

	void event_process();

	void add_io_event(int fd, io_callback* proc, int mask, void* args);

	void del_io_event(int fd);

	// 删除一个io事件的某个触发条件(EPOLLIN / EPOLLOUT)
	void del_io_event(int fd, int mask);

private:
	int _epfd;

	io_event_map _io_evs;

	listen_fd_set listen_fds;

	// 每次epoll_wait返回触发, 所返回的被激活的事件集合
	struct epoll_event _fired_evs[MAXEVENTS];

};
