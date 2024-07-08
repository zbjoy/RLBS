#pragma once

#include <queue>
#include <stdio.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <pthread.h>
#include "event_loop.h"

template <typename T>
class thread_queue
{
public:
	thread_queue()
	{
		_loop = NULL;
		pthread_mutex_init(&_queue_mutex, NULL);

		// 创建一个fd, 只能被epoll监听, 没有和磁盘相关联, 也没有和socket相关联
		_evfd = eventfd(0, EFD_NONBLOCK);
		if (_evfd == -1)
		{
			perror("(eventfd() error)");		
			exit(1);
		}
	}

	~thread_queue()
	{
		pthread_mutex_destroy(&_queue_mutex);
		close(_evfd);
	}

	void send(const T& task)
	{
		// 将task加入到queue中, 激活_evfd
		pthread_mutex_lock(&_queue_mutex);
		_queue.push(task);

		// 激活_evfd可读事件, 向_evfd写数据
		unsigned long long idle_num = 1; // 使用unsigned long long 类型是因为害怕数据太小导致epoll没有被触发
		int ret = write(_evfd, &idle_num, sizeof(unsigned long long));
		if (ret == -1)
		{
			perror("(error) evfd write error");		
		}
		pthread_mutex_unlock(&_queue_mutex);
	}

	void recv(std::queue<T>& queue_msgs)
	{
		unsigned long long idle_num;

		pthread_mutex_lock(&_queue_mutex);

		int ret = read(_evfd, &idle_num, sizeof(unsigned long long));
		if (ret == -1)
		{
			perror("(error)_evfd read");		
		}

		// 交换两个容器的指针, 保证queue_msgs是一个空队列
		std::swap(queue_msgs, _queue);

		pthread_mutex_unlock(&_queue_mutex);
	}

	void set_loop(event_loop* loop)
	{
		this->_loop = loop;
	}

	void set_callback(io_callback* cb, void* args = NULL)
	{
		if (_loop != NULL)
		{
			_loop->add_io_event(_evfd, cb, EPOLLIN, args);		
		}
	}

private:
	int _evfd; // 让某个线程监听的
	event_loop* _loop; // 目前是被哪个loop监听
	std::queue<T> _queue; // 队列
	pthread_mutex_t _queue_mutex; // 保护queue的互斥锁
};
