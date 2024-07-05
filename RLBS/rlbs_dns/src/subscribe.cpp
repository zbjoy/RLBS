#include "subscribe.h"
#include "rlbs.pb.h"

extern tcp_server* server;

SubscribeList* SubscribeList::_instance = NULL;

pthread_once_t SubscribeList::_once = PTHREAD_ONCE_INIT;

SubscribeList::SubscribeList()
{
	pthread_mutex_init(&_book_list_lock, NULL);	
	pthread_mutex_init(&_push_list_lock, NULL);
}

// 订阅功能
void SubscribeList::subscribe(uint64_t mod, int fd)
{
	pthread_mutex_lock(&_book_list_lock);	

	_book_list[mod].insert(fd);
	
	pthread_mutex_unlock(&_book_list_lock);
}

// 取消订阅
void SubscribeList::unsubscribe(uint64_t mod, int fd)
{
	// 将 mod-->fd 对应关系从 _book_list 中删除
	pthread_mutex_lock(&_book_list_lock);	
	if (_book_list.find(mod) != _book_list.end())
	{
		_book_list[mod].erase(fd);	// 将fd从set中删除
		if (_book_list[mod].empty() == true)
		{
			// 删除 map 的一对 kv 数据
			_book_list.erase(mod);		
		}
	}
	pthread_mutex_unlock(&_book_list_lock);
}

void SubscribeList::make_publish_map(listen_fd_set& online_fds, publish_map& need_publish)
{
	publish_map::iterator it;	

	pthread_mutex_lock(&_push_list_lock);

#if 0
		printf("hello before\n");
#endif

	for (it = _push_list.begin(); it != _push_list.end(); it++)
	{
#if 0
		printf("hello\n");
#endif
		if (online_fds.find(it->first) != online_fds.end())		
		{
			__gnu_cxx::hash_set<uint64_t>::iterator st;		

			for (st = _push_list[it->first].begin(); st != _push_list[it->first].end(); st++)
			{
				need_publish[it->first].insert(*st);		
			}
		}
	}

	pthread_mutex_unlock(&_push_list_lock);
}

void push_change_task(event_loop* loop, void* args)
{
	SubscribeList* subscribe = (SubscribeList*)args;	

#if 0
	printf("push_change_task\n");
#endif

	// 获取全部在线的客户端fd
	listen_fd_set online_fds;
	loop->get_listen_fds(online_fds);

	publish_map need_publish;
	need_publish.clear();
	subscribe->make_publish_map(online_fds, need_publish);

	publish_map::iterator it;

	for (it = need_publish.begin(); it != need_publish.end(); it++)
	{
		int fd = it->first;		

		__gnu_cxx::hash_set<uint64_t>::iterator st;
		for (st = it->second.begin(); st != it->second.end(); st++)
		{
			int modid = int((*st) >> 32);
			int cmdid = int(*st);

			rlbs::GetRouteResponse rsp;

			rsp.set_modid(modid);
			rsp.set_cmdid(cmdid);

			host_set hosts = Route::instance()->get_hosts(modid, cmdid);
			for (host_set_it hit = hosts.begin(); hit != hosts.end(); hit++)
			{
				uint64_t ip_port_pair = *hit;		
				rlbs::HostInfo host_info;
				host_info.set_ip((uint32_t)(ip_port_pair >> 32));
				host_info.set_port((int)(ip_port_pair));

				// 将 host_info 加入 rsp
				rsp.add_host()->CopyFrom(host_info);
			}

			std::string responseString;
			rsp.SerializeToString(&responseString);

			net_connection* conn = tcp_server::conns[fd];
			if (conn != NULL)
			{
				conn->send_message(responseString.c_str(), responseString.size(), rlbs::ID_GetRouteResponse);		

			}
			else
			{
				printf("publish conn == NULL! error fd = %d\n", fd);		
			}

			SubscribeList::instance()->get_push_list()->erase(fd);
		}
	}

}

// 发布功能
void SubscribeList::publish(std::vector<uint64_t>& change_mods)
{
	pthread_mutex_lock(&_book_list_lock);	
	pthread_mutex_lock(&_push_list_lock);	

	std::vector<uint64_t>::iterator it;

	for (it = change_mods.begin(); it != change_mods.end(); it++)
	{
		uint64_t mod = *it;		

		if (_book_list.find(mod) != _book_list.end())
		{
#if 0
				printf("hello\n");
#endif
			__gnu_cxx::hash_set<int>::iterator fds_it;		
			for (fds_it = _book_list[mod].begin(); fds_it != _book_list[mod].end(); fds_it++)
			{
#if 0
				printf("hello\n");
#endif
				int fd = *fds_it;		
				_push_list[fd].insert(mod);
			}
		}
	}

	pthread_mutex_unlock(&_push_list_lock);
	pthread_mutex_unlock(&_book_list_lock);

	server->get_th_pool()->send_task(push_change_task, this);
}
