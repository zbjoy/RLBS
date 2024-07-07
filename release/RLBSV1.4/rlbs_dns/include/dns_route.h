#pragma once

#include <pthread.h>
#include "mysql.h"
#include <ext/hash_map>
#include <ext/hash_set>

using __gnu_cxx::hash_map;
using __gnu_cxx::hash_set;

// 用来保存 host IP/port 的集合类型
typedef hash_set<uint64_t> host_set;
typedef hash_set<uint64_t>::iterator host_set_it;


// 用来保存 modID/cmdID 与 host IP/port 的对应数据类型
typedef hash_map<uint64_t, host_set> route_map; 
typedef hash_map<uint64_t, host_set>::iterator route_map_it;


class Route
{
public:	

	// 懒汉模式
	static Route* instance()
	{
		pthread_once(&_once, init);		
		return _instance;
	}

	// 连接数据库的方法
	void connect_db();

	// 构建 map route 数据的方法
	// 把RouteData表中的数据加载到内存中
	void build_maps();

	// 通过 modID/cmdID 获取全部的当前模块所挂载的host集合
	host_set get_hosts(int modid, int cmdid);

private:
	static void init()
	{
		_instance = new Route();		
	}

	Route();
	Route(const Route&);
	const Route& operator=(const Route&);

	static pthread_once_t _once;

	static Route* _instance;

	MYSQL _db_conn;

	// 指向RouteDataMap_A 当前关系的map
	route_map* _data_pointer;

	// 指向RouteDataMap_B 临时关系的map
	route_map* _temp_pointer;

	// map 读写锁
	pthread_rwlock_t _map_lock;

};

void* publish_change_mod_test(void* args);
