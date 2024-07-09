#include "main_server.h"
#include "udp_server.h"

void report_cb(const char* data, uint32_t len, int msgid, net_connection* conn, void* user_data)
{
	rlbs::ReportRequest req;
	req.ParseFromArray(data, len);

	route_lb* route_lb_p = (route_lb*)user_data;

	printf("udp server call report_cb\n");
}

void get_host_cb(const char* data, uint32_t len, int msgid, net_connection* conn, void* user_data)
{
	rlbs::GetHostRequest req;
	req.ParseFromArray(data, len);

	int modid = req.modid();
	int cmdid = req.cmdid();

	// 设置回复的消息
	rlbs::GetHostResponse rsp;
	rsp.set_seq(req.seq());
	rsp.set_modid(modid);
	rsp.set_cmdid(cmdid);

	// 通过 route_lb 获取一个可用 host 添加到rsp 中
	route_lb* route_lb_p = (route_lb*)user_data;
	route_lb_p->get_host(modid, cmdid, rsp);

	// 将 rsp 发送回给 api
	std::string responseString;
	rsp.SerializeToString(&responseString);
	conn->send_message(responseString.c_str(), responseString.size(), rlbs::ID_GetHostResponse);
}

void* agent_server_main(void* args)
{
	long index = (long)args;	

	short port = index + 8888;

	event_loop loop;

	udp_server server(&loop, "0.0.0.0", port);

	// 给 udp server 注册一些消息路由业务
	
	// 针对 API 的获取主机信息接口
	server.add_msg_router(rlbs::ID_GetHostRequest, get_host_cb, r_lb[port - 8888]); 

	// 针对 API 的上报主机调用结果接口
	server.add_msg_router(rlbs::ID_ReportRequest, report_cb, r_lb[port - 8888]);

	printf("agent UDP server : port %d is started...\n", port);

	loop.event_process();

	return NULL;
}


void start_UDP_servers(void)
{
	for (long i = 0; i < 3; i++)
	{
		pthread_t tid;

		int ret = pthread_create(&tid, NULL, agent_server_main, (void*)i);
		if (ret == -1)
		{
			perror("pthread create udp error\n");
			exit(1);
		}

		pthread_detach(tid);
	}
}
