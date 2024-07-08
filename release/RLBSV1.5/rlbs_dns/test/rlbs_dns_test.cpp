#include "rlbs_reactor.h"
#include "rlbs.pb.h"

void deal_get_route(const char* data, uint32_t len, int msgid, net_connection* conn, void* user_data)
{
	rlbs::GetRouteResponse rsp;
	rsp.ParseFromArray(data, len);

	printf("modid = %d\n", rsp.modid());
	printf("cmdid = %d\n", rsp.cmdid());
	printf("host_size = %d\n", rsp.host_size());

	for (int i = 0; i < rsp.host_size(); i++)
	{
		printf("---> ip = %u\n", rsp.host(i).ip());		
		printf("---> port = %d\n", rsp.host(i).port());
	}
}

void on_connection(net_connection* conn, void* args)
{
	rlbs::GetRouteRequest req;

	req.set_modid(1);
	req.set_cmdid(2);

	std::string requestString;

	req.SerializeToString(&requestString);
	conn->send_message(requestString.c_str(), requestString.size(), rlbs::ID_GetRouteRequest);
}

int main(int argc, char** argv)
{
	event_loop loop;

	tcp_client* client;

	client = new tcp_client(&loop, "127.0.0.1", 7777);

	client->set_conn_start(on_connection);

	client->add_msg_router(rlbs::ID_GetRouteResponse, deal_get_route);

	loop.event_process();
}
