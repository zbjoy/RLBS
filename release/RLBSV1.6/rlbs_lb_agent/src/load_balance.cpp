#include "load_balance.h"
#include "main_server.h"

int load_balance::pull()
{
	rlbs::GetRouteRequest req;
	req.set_modid(_modid);
	req.set_cmdid(_cmdid);

	// 将这个包发送 dns_queue
	dns_queue->send(req);

	status = PULLING;

	return 0;
	
}
