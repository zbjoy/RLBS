#include "rlbs_reactor.h"
#include "dns_route.h"
#include "subscribe.h"
#include "rlbs.pb.h"


tcp_server* server;

// Agent客户端已经订阅的mod模块集合
typedef __gnu_cxx::hash_set<uint64_t> client_sub_mod_list;

// 处理 Agent 发送 Route 信息获取的业务
void get_route(const char* data, uint32_t len, int msgid, net_connection* conn, void* user_data)
{
	// 解析 proto 文件
	rlbs::GetRouteRequest req;
	req.ParseFromArray(data, len);

	// 得到 modID 与 cmdID
	int modid, cmdid;
	modid = req.modid();
	cmdid = req.cmdid();

	// 如果之前没有订阅过 modid/cmdid, 则订阅
	uint64_t mod = (((uint64_t)modid) << 32) + cmdid;
	client_sub_mod_list* sub_list = (client_sub_mod_list*)conn->param;
	if (sub_list == NULL)
	{
		fprintf(stderr, "sub_list = NULL\n");		
	}
	else if (sub_list->find(mod) == sub_list->end())
	{
		sub_list->insert(mod);		

		SubscribeList::instance()->subscribe(mod, conn->get_fd());
		printf("fd %d subscribe modid = %d, cmdid = %d\n", conn->get_fd(), modid, cmdid);
	}

	// 通过 modID/cmdID 获得 host 信息从 _data_pointer 所指向的 map 中
	host_set hosts = Route::instance()->get_hosts(modid, cmdid);

	// 打包一个新的 response protobuf 数据
	rlbs::GetRouteResponse rsp;
	rsp.set_modid(modid);
	rsp.set_cmdid(cmdid);

	for (host_set_it it = hosts.begin(); it != hosts.end(); it++)
	{
		uint64_t ip_port = *it;		

		rlbs::HostInfo host;

		host.set_ip((uint32_t)(ip_port >> 32));
		host.set_port((uint32_t)(ip_port));

		// 将 host 添加到 rsp 对象中
		rsp.add_host()->CopyFrom(host);

	}

	std::string responseString;
	rsp.SerializeToString(&responseString);
	conn->send_message(responseString.c_str(), responseString.size(), rlbs::ID_GetRouteResponse);


}

// 每个新客户端创建成功之后, 执行该函数
void create_subscribe(net_connection* conn, void* args)
{
	conn->param = new client_sub_mod_list;	
}

void clear_subscribe(net_connection* conn, void* args)
{
	client_sub_mod_list::iterator it;	
	client_sub_mod_list* sub_list = (client_sub_mod_list*)conn->param;

	for (it = sub_list->begin(); it != sub_list->end(); it++)
	{
		uint64_t mod = *it;		
		SubscribeList::instance()->unsubscribe(mod, conn->get_fd());
	}

	delete sub_list;

	conn->param = NULL;
}

int main()
{
	event_loop loop;	

	config_file::setPath("../conf/rlbs_dns.ini");
	std::string ip = config_file::instance()->GetString("reactor", "ip", "127.0.0.1");
	short port = config_file::instance()->GetNumber("reactor", "port", 7777);

	server = new tcp_server(&loop, ip.c_str(), port);

	server->set_conn_start(create_subscribe);
	server->set_conn_close(clear_subscribe);

	// 注册一个回调业务
	server->add_msg_router(rlbs::ID_GetRouteRequest, get_route);

	pthread_t tid;
	int ret = pthread_create(&tid, NULL, check_route_changes, NULL);
	if (ret == -1)
	{
		perror("pthread_create error\n");
		exit(1);
	}

	pthread_detach(tid);

	loop.event_process();

	return 0;
}
