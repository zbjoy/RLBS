#include "rlbs_reactor.h"
#include "rlbs.pb.h"
#include "store_report.h"

tcp_server* server;

thread_queue<rlbs::ReportStatusRequest>** reportQueues = NULL;
int thread_cnt;

void get_report_status(const char* data, uint32_t len, int msgid, net_connection* conn, void* user_data)
{
	rlbs::ReportStatusRequest req;	
	req.ParseFromArray(data, len);

	// 将当前的请求入库的交给一个线程来处理
	static int index = 0;
	reportQueues[index]->send(req);
	index++;
	index = index % thread_cnt;
}

void create_reportdb_threads(void)
{
	thread_cnt = config_file::instance()->GetNumber("reporter", "db_thread_cnt", 3);

	// 线程的消息队列
	reportQueues = new thread_queue<rlbs::ReportStatusRequest>* [thread_cnt];
	if (reportQueues == NULL)
	{
		fprintf(stderr, "create thread queue error\n");		
		exit(1);
	}

	for (int i = 0; i < thread_cnt; i++)
	{
		reportQueues[i] = new thread_queue<rlbs::ReportStatusRequest>;
		if (reportQueues[i] == NULL)
		{
			fprintf(stderr, "create thread queue %d error\n", i);		
			exit(1);
		}
		pthread_t tid;
		int ret = pthread_create(&tid, NULL, store_main, reportQueues[i]);
		if (ret == -1)
		{
			perror("pthread create");
			exit(1);
		}

		pthread_detach(tid);
	}



}

int main()
{
	event_loop loop;	

	config_file::setPath("../conf/rlbs_reporter.ini");
	std::string ip = config_file::instance()->GetString("reactor", "ip", "127.0.0.1");
	short port = config_file::instance()->GetNumber("reactor", "port", 8888);

	server = new tcp_server(&loop, ip.c_str(), port);

	// 添加上报请求的消息分发业务
	server->add_msg_router(rlbs::ID_ReportStatusRequest, get_report_status);

	create_reportdb_threads();

	loop.event_process();

	return 0;
}
