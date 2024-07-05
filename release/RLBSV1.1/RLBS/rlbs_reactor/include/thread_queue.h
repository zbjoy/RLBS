#pragma once

#include <queue>
#include <pthread.h>
#include <stdio.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include "event_loop.h"

template <typename T>
class thread_queue
{
public:
	thread_queue()
	{
		_loop = NULL;
		pthread_mutex_init(&_queue_mutex, NULL);
		
		_evfd = eventfd(0, EFD_NONBLOCK);
		if (_evfd == -1)
		{
			perror("eventfd()");		
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
		pthread_mutex_lock(&_queue_mutex);
		_queue.push(task);

		unsigned long long idle_num = 1;
		int ret = write(_evfd, &idle_num, sizeof(unsigned long long));
		if (ret == -1)
		{
			perror("evfd write error");		
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
			perror("_evfd read");		
		}

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
	int _evfd;
	event_loop* _loop;
	std::queue<T> _queue;
	pthread_mutex_t _queue_mutex;

};
