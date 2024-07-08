#include "store_report.h"
#include "rlbs.pb.h"
#include "rlbs_reactor.h"
#include <queue>

struct Args
{
	thread_queue<rlbs::ReportStatusRequest>* queue;	
	StoreReport* store_conn;
};

//从消息队列中取出上报数据, 进行入库
void thread_report(event_loop* loop, int fd, void* args)
{
	thread_queue<rlbs::ReportStatusRequest>* queue = ((Args*)args	)->queue;
	StoreReport* store_conn = ((Args*)args)->store_conn;

	std::queue<rlbs::ReportStatusRequest> report_msgs;

	// 需要从队列中取出数据入库
	queue->recv(report_msgs);
	while (!report_msgs.empty())
	{
		rlbs::ReportStatusRequest msg = report_msgs.front();		
		report_msgs.pop();

		store_conn->store(msg);
		StoreReport sr;
		sr.store(msg);
	}
}

// 每个存储线程的主业务
void* store_main(void* args)
{
	thread_queue<rlbs::ReportStatusRequest>* queue = (thread_queue<rlbs::ReportStatusRequest>*)args;	

	event_loop loop;

	// 定义一个存储对象, 每个线程创建一个长连接存储对象
	StoreReport sr;

	Args callback_args;
	callback_args.queue = queue;
	callback_args.store_conn = &sr;

	queue->set_loop(&loop);
	queue->set_callback(thread_report, &callback_args);

	loop.event_process();

	return NULL;
}
