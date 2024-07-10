#pragma once

#include "load_balance.h"

typedef __gnu_cxx::hash_map<uint64_t, load_balance*> route_map;
typedef __gnu_cxx::hash_map<uint64_t, load_balance*>::iterator route_map_it;

/*
 *	针对多组 modid/cmdid 和 load_balance 的对应关系
 *
 *	目前 route_lb 对象只有 3 个, 每个 udp server 都有一个 route_lb 对象
 *
 * */

class route_lb
{
public:
	route_lb(int id);	

	// agent 获取一个 host 主机, 将返回的主机结果存放在 rsp 中
	int get_host(int modid, int cmdid, rlbs::GetHostResponse& rsp);

	// agent 获取 host 主机集合 将返回的主机结果存放到 rsp 中
	int get_route(int modid, int cmdid, rlbs::GetRouteResponse& rsp);

	int update_host(int modid, int cmdid, rlbs::GetRouteResponse& rsp);

	void report_host(rlbs::ReportRequest& req);

	void reset_lb_status();

private:
	route_map _route_lb_map; // 当前 route_lb 模块所管理的 loadbalance 集合
	pthread_mutex_t _mutex;
	int _id; // 当前 route_lb 的编号 和 udp server 是 一一对应的
};
