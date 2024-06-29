#include "tcp_client.h"

void callback_busi(const char* data, uint32_t len, int msgid, net_connection* conn, void* user_data)
{
#if 0
	printf("recv from server\n");	
	printf("msgid = %d\n", msgid);
	printf("len = %d\n", len);
	printf("data = %s\n", data);
	printf("====================\n");
#endif

	conn->send_message(data, len, msgid);
}

void print_busi(const char* data, uint32_t len, int msgid, net_connection* conn, void* user_data)
{
	printf("print busi is called!!\n");
	printf("recv from server : [%s]\n", data);
	printf("msgid = %d\n", msgid);
	printf("len = %d\n", len);
}

int main()
{
	event_loop loop;	

	tcp_client* client = new tcp_client(&loop, "127.0.0.1", 7777);

	// client->set_msg_callback(busi);
	client->add_msg_router(1, print_busi);
	client->add_msg_router(2, callback_busi);

	loop.event_process();
}
