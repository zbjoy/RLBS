#include "rlbs_reactor.h"
#include "dns_route.h"
#include "rlbs.pb.h"

void get_route(const char* data, uint32_t len, int msgid, net_connection* conn, void* user_data)
{
	// 解析 proto 文件
	rlbs::GetRouteRequest req;
	req.ParseFromArray(data, len);

	// 得到 modID 与 cmdID
	int modid, cmdid;
	modid = req.modid();
	cmdid = req.cmdid();

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

int main()
{
	event_loop loop;	

	config_file::setPath("../conf/rlbs_dns.ini");
	std::string ip = config_file::instance()->GetString("reactor", "ip", "127.0.0.1");
	short port = config_file::instance()->GetNumber("reactor", "port", 7777);

	tcp_server* server = new tcp_server(&loop, ip.c_str(), port);

	// 注册一个回调业务
	server->add_msg_router(rlbs::ID_GetRouteRequest, get_route);

	loop.event_process();

	return 0;
}
