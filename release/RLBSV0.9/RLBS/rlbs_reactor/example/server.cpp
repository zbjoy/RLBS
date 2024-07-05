#include "tcp_server.h"
#include <string.h>
#include "config_file.h"

void callback_busi(const char* data, uint32_t len, int msgid, net_connection* conn, void* user_data)
{
	printf("callback busi is called!!\n");	

	conn->send_message(data, len, msgid);
}

void print_busi(const char* data, uint32_t len, int msgid, net_connection* conn, void* user_data)
{
	printf("print busi is called!!\n");	
	printf("recv from client : [%s]\n", data);
	printf("msgid = %d\n", msgid);
	printf("len = %d\n", len);
}

// 新创建客户端成功之后的回调
void on_client_build(net_connection* conn, void* args)
{
	printf("===> on_client_build is called!!\n");	
	int msgid = 200;
	const char* msg = "welcome!! you are online!!!\n";

	conn->send_message(msg, strlen(msg), msgid);
}

void on_client_lost(net_connection* conn, void* args)
{
	printf("===> on_client_lost is called!!\n");	
	printf("cpnnection is lost!! \n");

}

int main()
{
	config_file::setPath("./reactor.ini");
	std::string ip = config_file::instance()->GetString("reactor", "ip", "0.0.0.0");
	short port = config_file::instance()->GetNumber("reactor", "port", 8888);

	event_loop loop;
	tcp_server server(&loop, ip.c_str(), port);
	// server.do_accept();

	server.add_msg_router(1, callback_busi);
	server.add_msg_router(2, print_busi);

	// 注册Hook函数
	server.set_conn_start(on_client_build);
	server.set_conn_close(on_client_lost);

	loop.event_process();
	return 0;	
}
