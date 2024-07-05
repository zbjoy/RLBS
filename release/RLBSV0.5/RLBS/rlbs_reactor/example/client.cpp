#include "tcp_client.h"

void busi(const char* data, uint32_t len, int msgid, tcp_client* client, void* user_data)
{
	printf("recv from server\n");	
	printf("msgid = %d\n", msgid);
	printf("len = %d\n", len);
	printf("data = %s\n", data);
	printf("====================\n");
}

int main()
{
	event_loop loop;	

	tcp_client* client = new tcp_client(&loop, "127.0.0.1", 7777);

	client->set_msg_callback(busi);

	loop.event_process();
}
