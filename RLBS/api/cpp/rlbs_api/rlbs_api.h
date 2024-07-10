#pragma once

#include "rlbs_reactor.h"
#include <string>
#include <vector>

typedef std::pair<std::string, int> ip_port;
typedef std::vector<ip_port> route_set;
typedef route_set::iterator route_set_it;

class rlbs_client
{
public:
	rlbs_client();
	~rlbs_client();

	// 注册一个模块 (一个模块调用一次)
	int reg_init(int modid, int cmdid);

	// rlbs 系统获取 host 信息, 得到可用的 host ip 和 port api
	int get_host(int modid, int cmdid, std::string& ip, int& port);

	// rlbs 获取某个 modid/cmdid 的全部的 hosts
	int get_route(int modid, int cmdid, route_set& route);

	void report(int modid, int cmdid, std::string& ip, int port, int retcode);



private:
	int _sockfd[3];
	uint32_t _seqid;

};
