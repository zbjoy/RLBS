#pragma once

#include <vector>
#include <pthread.h>
#include <ext/hash_map>
#include <ext/hash_set>
#include "dns_route.h"
#include "rlbs_reactor.h"

// using namespace __gnu_cxx;

// 定义一个订阅列表的关系类型, key->modID/cmdID, value->fds(集合)
typedef __gnu_cxx::hash_map<uint64_t, __gnu_cxx::hash_set<int>> subscribe_map;

// 定义一个发布列表的关系类型, key->fd(订阅客户端的连接), value->modID/cmdID
typedef __gnu_cxx::hash_map<int, __gnu_cxx::hash_set<uint64_t>> publish_map;

class SubscribeList
{
public:

	// 饿汉模式
	static SubscribeList* instance()
	{
		pthread_once(&_once, init);		
		return _instance;
	}

	// 订阅功能
	void subscribe(uint64_t mod, int fd);

	// 取消订阅功能
	void unsubscribe(uint64_t mod, int fd);

	// 发布功能
	// 输入形参: 被修改的modid, 被修改的模块所对应的fd就应该被发布, 收到新的modID/cmdID的结果
	void publish(std::vector<uint64_t>& change_mods);
	
	void make_publish_map(listen_fd_set& online_fds, publish_map& need_publish);

	publish_map* get_push_list()
	{
		return& _push_list;		
	}


private:

	static void init()
	{
		_instance = new SubscribeList();		
	}

	SubscribeList();
	SubscribeList(const SubscribeList&);
	const SubscribeList& operator=(const SubscribeList&);
	static SubscribeList* _instance;
	static pthread_once_t _once;

	subscribe_map _book_list;
	pthread_mutex_t _book_list_lock;

	publish_map _push_list;
	pthread_mutex_t _push_list_lock;

};

