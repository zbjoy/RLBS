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

void load_balance::update(rlbs::GetRouteResponse& rsp)
{
	std::set<uint64_t> remote_hosts;	
	std::set<uint64_t> need_delete;

	// 1 确保 dns service 返回的结果有 host 信息
	assert(rsp.host_size() != 0);

	// 2 插入新增的 host 信息到 _host_map 中
	for (int i = 0; i < rsp.host_size(); i++)
	{
		const rlbs::HostInfo& h = rsp.host(i);

		// 得到主机的 ip + port key 值
		uint64_t key = ((uint64_t)h.ip() << 32) + h.port();

		// 将远程的主机加入到 remote_hosts 集合中
		remote_hosts.insert(key);

		if (_host_map.find(key) == _host_map.end())
		{
			host_info* hi = new host_info(h.ip(), h.port());
			if (hi == NULL)
			{
				fprintf(stderr, "new host_info error\n");
				exit(1);
			}
			_host_map[key] = hi;

			// 将新增的 host 信息加入到空闲列表中
			_idle_list.push_back(hi);
		}
	}

	// 3 遍历本地 map 和远程 dns 返回的主机集合, 得到需要被删除的主机
	for (host_map_it it = _host_map.begin(); it != _host_map.end(); it++)
	{
		if (remote_hosts.find(it->first) == remote_hosts.end())
		{
			// 代表当前主机, 没有在远程的主机中存在, 说明当前主机已经被修改或删除
			need_delete.insert(it->first);
		}
	}

	// 4 删除主机
	for (std::set<uint64_t>::iterator it = need_delete.begin(); it != need_delete.end(); it++)
	{
		uint64_t key = *it;

		host_info* hi = _host_map[key];

		if (hi->overload == true)
		{
			// 从过载队列中删除
			_overload_list.remove(hi);
		}
		else
		{
			// 从空闲队列中删除
			_idle_list.remove(hi);
		}

		delete hi;
	}

}

// 从 idle_list 或者 overload_list 中获得一个 host 节点信息
void get_host_from_list(rlbs::GetHostResponse& rsp, host_list& l)
{
	// 从 list 中选择第一个节点
	host_info* host = l.front();

	// 将取出的 host 节点信息添加给 rsp 的 hostInfo 字段
	rlbs::HostInfo* hip = rsp.mutable_host();
	hip->set_ip(host->ip);
	hip->set_port(host->port);

	l.pop_front();
	l.push_back(host);

}

int load_balance::choice_one_host(rlbs::GetHostResponse& rsp)
{
	if (_idle_list.empty() == true)
	{
		if (_access_cnt >= lb_config.probe_num)
		{
			_access_cnt = 0;

			// 从 overload_list 选择一个过载的节点
			get_host_from_list(rsp, _overload_list);
		}
		else
		{
			++_access_cnt;
			return rlbs::RET_OVERLOAD;
		}
	}
	else
	{
		if (_overload_list.empty() == true)
		{
			get_host_from_list(rsp, _idle_list);
		}
		else
		{
			if (_access_cnt >= lb_config.probe_num)
			{
				_access_cnt = 0;

				get_host_from_list(rsp, _overload_list);
			}
			else
			{
				++_access_cnt;

				get_host_from_list(rsp, _idle_list);
			}
		}
	}

	return rlbs::RET_SUCC;
}
