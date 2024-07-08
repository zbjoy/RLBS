#pragma once

#include <stdint.h>

struct host_info
{
	host_info(uint32_t ip, int port) :
				ip(ip),
				port(port),
				vsucc(0),
				verr(0),
				rsucc(0),
				rerr(0),
				contin_succ(0),
				contin_err(0),
				overload(false)
	{

	}

	uint32_t ip;
	int port;

	uint32_t vsucc;
	uint32_t verr;
	uint32_t rsucc;
	uint32_t rerr;

	uint32_t contin_succ;
	uint32_t contin_err;

	bool overload;
};
