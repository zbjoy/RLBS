#pragma once

#include <stdint.h>
#include <stdio.h>
#include <time.h>

struct host_info
{
	host_info(uint32_t ip, int port, int init_succ) :
				ip(ip),
				port(port),
				vsucc(init_succ),
				verr(0),
				rsucc(0),
				rerr(0),
				contin_succ(0),
				contin_err(0),
				overload(false)
	{
		idle_ts = time(NULL);
	}

	void set_overload();
	void set_idle();

	uint32_t ip;
	int port;

	uint32_t vsucc;
	uint32_t verr;
	uint32_t rsucc;
	uint32_t rerr;

	uint32_t contin_succ;
	uint32_t contin_err;

	long idle_ts; // 当前主机变更为 idle 状态的时间
	long overload_ts; // 当主机变更为 overload 状态的时间
	bool overload;
};
