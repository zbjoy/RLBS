#pragma once

#include <ext/hash_map>
#include <ext/hash_set>
#include <sys/epoll.h>
#include <vector>
#include "event_base.h"

#define MAXEVENTS 10

typedef __gnu_cxx::hash_map<int, io_event> io_event_map;
typedef __gnu_cxx::hash_map<int, io_event>::iterator io_event_map_it;

typedef __gnu_cxx::hash_set<int> listen_fd_set;

typedef void (*task_func)(event_loop* loop, void* args);

class event_loop
{
public:	
	event_loop();

	void event_process();

	void add_io_event(int fd, io_callback* proc, int mask, void* args);

	void del_io_event(int fd);
	void del_io_event(int fd, int mask);

	// ==================== 对于 异步task任务的方法 =========================
	/* 添加一个task任务到_ready_tasks集合中 */
	void add_task(task_func func, void* args);

	void execute_ready_tasks();

	void get_listen_fds(listen_fd_set& fds)
	{
		fds = listen_fds;		
	}

private:
	int _epfd;

	/* 当前event_loop 监控的fd和对应的事件关系 */
	io_event_map _io_evs;

	listen_fd_set listen_fds;

	struct epoll_event _fired_evs[MAXEVENTS];

	/* 针对 异步task任务的属性 */
	typedef std::pair<task_func, void*> task_func_pair;

	std::vector<task_func_pair> _ready_tasks;
};
