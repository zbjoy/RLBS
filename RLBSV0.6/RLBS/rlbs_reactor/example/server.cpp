#include "tcp_server.h"

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

int main()
{
	event_loop loop;
	tcp_server server(&loop, "127.0.0.1", 7777);
	// server.do_accept();

	server.add_msg_router(1, callback_busi);
	server.add_msg_router(2, print_busi);

	loop.event_process();
	return 0;	
}
