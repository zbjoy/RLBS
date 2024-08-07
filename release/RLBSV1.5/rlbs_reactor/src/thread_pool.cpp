#include "thread_pool.h"
#include <stdio.h>

thread_queue<task_msg>* thread_pool::get_thread()
{
	if (_index == _thread_cnt)
	{
		_index = 0;		
	}

	printf("===> thread_num = %d <===\n", _index);
	return _queues[_index++];
}

void deal_task(event_loop* loop, int fd, void*args)
{
	thread_queue<task_msg>* queue = (thread_queue<task_msg>*)args;	

	std::queue<task_msg> new_task_queue;
	queue->recv(new_task_queue);

	while (new_task_queue.empty() != true)
	{
		task_msg task = new_task_queue.front();

		new_task_queue.pop();

		if (task.type == task_msg::NEW_CONN)
		{
			tcp_conn* conn = new tcp_conn(task.connfd, loop);		

			if (conn == NULL)
			{
				fprintf(stderr, "in thread new tcp_conn error\n");		
				exit(1);
			}

			printf("[thread]: create net conntecion success!!\n");
		}
		else if (task.type == task_msg::NEW_TASK)
		{
			loop->add_task(task.task_cb, task.args);		
		}
		else
		{
			fprintf(stderr, "unknow task!!\n");
		}
	}
}

void* thread_main(void* args)
{
	thread_queue<task_msg>* queue = (thread_queue<task_msg>*)args;

	event_loop* loop = new event_loop();
	if (loop == NULL)
	{
		fprintf(stderr, "new event_loop error\n");		
		exit(1);
	}

	queue->set_loop(loop);
	queue->set_callback(deal_task, queue);

	loop->event_process();

	return NULL;
}

thread_pool::thread_pool(int thread_cnt)
{
	_index =0;
	_queues = NULL;
	_thread_cnt = thread_cnt;
	if (_thread_cnt <= 0)
	{
		fprintf(stderr, "thread_cnt need > 0\n");		
		exit(1);
	}

	_queues = new thread_queue<task_msg>* [thread_cnt];

	_tids = new pthread_t[thread_cnt];

	int ret;
	for (int i = 0; i < thread_cnt; i++)
	{
		_queues[i] = new thread_queue<task_msg>();

		ret = pthread_create(&_tids[i], NULL, thread_main, _queues[i]);
		if (ret == -1)
		{
			perror("error: thread_pool create error");		
			exit(1);
		}

		printf("create %d thread\n", i);

		pthread_detach(_tids[i]);
	}

	printf("===> new thread_pool <===\n");

	return;
}


void thread_pool::send_task(task_func func, void* args)
{
	task_msg task;

	task.type = task_msg::NEW_TASK;
	task.task_cb = func;
	task.args = args;

	for (int i = 0; i < _thread_cnt; i++)
	{
		thread_queue<task_msg>* queue = _queues[i];		

		queue->send(task);
	}
}
