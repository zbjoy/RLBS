#pragma once

#include "host_info.h"
#include "rlbs.pb.h"
#include <ext/hash_map>
#include <list>

typedef __gnu_cxx::hash_map<uint64_t, host_info*> host_map;

typedef __gnu_cxx::hash_map<uint64_t, host_info*>::iterator host_map_it;

typedef std::list<host_info*> host_list;
typedef std::list<host_info*>::iterator host_list_it;

/*
 *
 * 针对一个 (modid/cmdid) 下的全部的 host 节点做负载规则
 * */
class load_balance
{
public:
	load_balance(int modid, int cmdid)	
	{
		_modid = modid;
		_cmdid = cmdid;		
		_access_cnt = 0;

		status = NEW;
		last_update_time = time(NULL);
	}

	bool empty()
	{
		return _host_map.empty();		
	}


	// 获得 host 主机集合
	void get_all_hosts(std::vector<host_info*>& vec);

	// 获得一个可用的 host 信息
	int choice_one_host(rlbs::GetHostResponse& srp);

	// 向远程的 DNS service 中发送 ID_GetRouteRequest 请求
	int pull();

	// 根据 Dns service 远程返回的主机结果, 更新自己的 host_map 表
	void update(rlbs::GetRouteResponse& rsp);

	void report(int ip, int port, int retcode);

	void commit();

	enum STATUS
	{
		PULLING, // 正在从远程 dns service 网络通信中
		NEW		 // 正在创建新的 load_balance 模块
	};

	STATUS status;

	long last_update_time;

private:
	int _modid;
	int _cmdid;

	// host_map 当前负载均衡模块所管理的全部主机
	host_map _host_map;

	// 空闲队列
	host_list _idle_list;

	// 过载队列
	host_list _overload_list;

	// 当前 modid/cmdid的请求次数
	int _access_cnt;
};
