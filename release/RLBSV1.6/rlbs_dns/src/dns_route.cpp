// #include "mysql.h"
#include "dns_route.h"
#include "config_file.h"
#include "subscribe.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

using namespace std;

Route* Route::_instance = NULL;
pthread_once_t Route::_once = PTHREAD_ONCE_INIT;

Route::Route() : _version(0)
{
	printf("Route init\n");	
	memset(_sql, 0, 1000);

	// 初始化锁
	pthread_rwlock_init(&_map_lock, NULL);
	

	// 初始化 map
	_data_pointer = new route_map(); // RouteDataMap_A
	_temp_pointer = new route_map(); // RouteDataMap_B

	this->connect_db();

	// 将数据库中的 RouteData 的数据加载到 _data_poiner 中
	this->build_maps();

}

int Route::load_version()
{
	memset(_sql, 0, 1000);	
	snprintf(_sql, 1000, "SELECT version from RouteVersion WHERE id = 1");
	int ret = mysql_real_query(&_db_conn, _sql, strlen(_sql));
	if (ret != 0)
	{
		fprintf(stderr, "SELECT RouteVersion error %s\n", mysql_error(&_db_conn));		
		exit(1);
	}

	MYSQL_RES* result = mysql_store_result(&_db_conn);

	// 得到行数
	long line_num = mysql_num_rows(result);
	if (line_num == 0)
	{
		fprintf(stderr, "No version in table Routeversion: %s\n", mysql_error(&_db_conn));		
		return -1;
	}

	MYSQL_ROW row = mysql_fetch_row(result);

	// 得到最新的version
	long new_version = atoi(row[0]);

	if (new_version == this->_version)
	{
		return 0;		
	}

	this->_version = new_version;

	printf("now route version is %ld\n", this->_version);

	mysql_free_result(result);

	return 1;
}

// 加载 RouteChange 得到修改的 modID/cmdID
// 放在vector中
void Route::load_changes(std::vector<uint64_t>& change_list)
{
	memset(_sql, 0, 1000);	

	// 查询最新版本数据
	snprintf(_sql, 1000, "SELECT modid, cmdid FROM RouteChange WHERE version >= %ld", this->_version);

	int ret = mysql_real_query(&_db_conn, _sql, strlen(_sql));
	if (ret != 0)
	{
		fprintf(stderr, "SELECT RouteChange error %s\n", mysql_error(&_db_conn));		
		exit(1);
	}

	// 获得一个结果集合
	MYSQL_RES* result = mysql_store_result(&_db_conn);
	
	// 获得行数
	long line_num = mysql_num_rows(result);
	if (line_num == 0)
	{
		fprintf(stderr, "no Change in RouteChange: %s\n", mysql_error(&_db_conn));		
		return;
	}

	MYSQL_ROW row;
	for (long i = 0; i < line_num; i++)
	{
		row = mysql_fetch_row(result);		
		int modid = atoi(row[0]);
		int cmdid = atoi(row[1]);

		uint64_t mod = (((uint64_t)modid) << 32) + cmdid;
		change_list.push_back(mod);
	}

	mysql_free_result(result);

	return;
}

void Route::build_maps()
{
	int ret = 0;	
	// char sql[1000];
	// memset(sql, 0, 1000);
	memset(_sql, 0, 1000);

	// 查询 RouteData 数据库
	snprintf(_sql, 1000, "SELECT * FROM RouteData");
	ret = mysql_real_query(&_db_conn, _sql, strlen(_sql));
	if (ret != 0)
	{
		fprintf(stderr, "select RouteData error %s\n", mysql_error(&_db_conn));		
		exit(1);
	}

	// 获得一个结果集合
	MYSQL_RES* result = mysql_store_result(&_db_conn);

	// 得到行数
	long line_num = mysql_num_rows(result);

	MYSQL_ROW row;
	for (int i = 0; i < line_num; i++)
	{
		// 处理一行数据		
		row = mysql_fetch_row(result);
		int modID = atoi(row[1]);
		int cmdID = atoi(row[2]);
		unsigned int ip = atoi(row[3]);
		int port = atoi(row[4]);

		printf("modid = %d, cmdid = %d, ip = %u, port = %d\n", modID, cmdID, ip, port);

		uint64_t key = ((uint64_t)modID << 32) + cmdID;
		uint64_t value = ((uint64_t)ip << 32) + port;

		// 插入到 RouteDataMap_A中
		(*_data_pointer)[key].insert(value);

	}

	mysql_free_result(result);
}

// 将 RouteData 表中的数据加载到 _temp_pointer
void Route::load_route_data()
{
	int ret = 0;	
	memset(_sql, 0, 1000);

	// 清空 temp_pointer 所指向的临时表
	_temp_pointer->clear();

	// 查询 RouteData 数据库
	// "Select * from RouteData"
	snprintf(_sql, 1000, "SELECT * FROM RouteData");
	ret = mysql_real_query(&_db_conn, _sql, strlen(_sql));
	if (ret != 0)
	{
		fprintf(stderr, "select RouteData error %s\n", mysql_error(&_db_conn));		
		exit(1);
	}

	// 获得一个结果集合
	MYSQL_RES* result = mysql_store_result(&_db_conn);

	// 获得行数
	long line_num = mysql_num_rows(result);

	// 遍历分析集合中的元素, 加入 _data_pointer 中(MapA中)
	MYSQL_ROW row;
	for (int i = 0; i < line_num; i++)
	{
		// 处理一行数据
		row = mysql_fetch_row(result);		
		int modID = atoi(row[1]);
		int cmdID = atoi(row[2]);
		unsigned int ip = atoi(row[3]);
		int port = atoi(row[4]);

		// 将读到的数据加入map中
		// 组装一个 map 的 key
		uint64_t key = ((uint64_t)modID << 32) + cmdID;
		uint64_t value = ((uint64_t)ip << 32) + port;

		// 插入到 RouteDataMap_B 中
		(*_temp_pointer)[key].insert(value);
	}

	mysql_free_result(result);
}

void Route::connect_db()
{
	string db_host = config_file::instance()->GetString("mysql", "db_host", "127.0.0.1");	
	short db_port = config_file::instance()->GetNumber("mysql", "db_port", 3306);
	string db_user = config_file::instance()->GetString("mysql", "db_user", "root");
	string db_passwd = config_file::instance()->GetString("mysql", "db_passwd", "123");
	string db_name = config_file::instance()->GetString("mysql", "db_name", "123");

	mysql_init(&_db_conn);

#if 0

	printf("db_host is %s\n", db_host.c_str());
	printf("db_port is %d\n", db_port);
	printf("db_user is %s\n", db_user.c_str());
	printf("db_passwd is %s\n", db_passwd.c_str());
	printf("db_name is %s\n", db_name.c_str());

#endif

	// 设置一个超时定期重连
	mysql_options(&_db_conn, MYSQL_OPT_CONNECT_TIMEOUT, "30");
	// 开启mysql断开重连机制
	// my_bool reconnect = 1;
	bool reconnect = 1;
	mysql_options(&_db_conn, MYSQL_OPT_RECONNECT, &reconnect);

	// 连接数据库
	if (!mysql_real_connect(&_db_conn, db_host.c_str(), db_user.c_str(), db_passwd.c_str(), db_name.c_str(), db_port, NULL, 0))
	{
		fprintf(stderr, "failed to connect mysql!!\n");		
		exit(1);
	}

	printf("connect db success!!\n");
}

host_set Route::get_hosts(int modid, int cmdid)
{
	host_set hosts;	

	uint64_t key = ((uint64_t)modid << 32) + cmdid;

	// 通过 map 获得
	pthread_rwlock_rdlock(&_map_lock);
	route_map_it it = _data_pointer->find(key);
	if (it != _data_pointer->end())
	{
		hosts = it->second;		
	}

	pthread_rwlock_unlock(&_map_lock);

	return hosts;
}

void Route::swap()
{
	pthread_rwlock_wrlock(&_map_lock);	
	route_map* temp = _data_pointer;
	_data_pointer = _temp_pointer;
	_temp_pointer = temp;
	pthread_rwlock_unlock(&_map_lock);
}

// 周期性后端检查 db 的 route 信息的更改业务
void* check_route_changes(void* args)
{
	int wait_time = 10;	 // 10s 自动加载 RouteData 一次
	long last_load_time = time(NULL);

	while (true)
	{
		sleep(1);		
		long current_time = time(NULL);

		int ret = Route::instance()->load_version();
		if (ret == 1)
		{
			// version 已经被更改, 有 modid/cmdid 被修改

			// 1 将最新的 RouteData 的数据加载到 _temp_pointer 中
			Route::instance()->load_route_data();
			
			// 2 将 _temp_pointer 的数据更新到 _data_pointer 中
			Route::instance()->swap();
			last_load_time = current_time;

			// 3 获取当前已经被修改的 modid/cmdid 集合 vector
			std::vector<uint64_t> changes;
			Route::instance()->load_changes(changes);

			// 4 给订阅修改的 mod 客户端 agent 推送消息
			SubscribeList::instance()->publish(changes);

			// TODO 将 RouteChanges 表清空
		}
		else
		{
			// version 没有被修改		
			if (current_time - last_load_time >= wait_time)
			{
				// 定期检查超时 强制加载 _temp_pointer-->_data_pointer		
				Route::instance()->load_route_data();
				Route::instance()->swap();
				last_load_time = current_time;
			}
		}
	}
	return NULL;
}


// test 发布的函数
void* publish_change_mod_test(void* args)
{
	while (true)	
	{
		sleep(1);		

		// printf("hello\n");

		int modid1 = 1;
		int cmdid1 = 1;
		uint64_t mod1 = (((uint64_t)modid1) << 32) + cmdid1;

		int modid2 = 1;
		int cmdid2 = 2;
		uint64_t mod2 = (((uint64_t)modid2) << 32) + cmdid2;

		std::vector<uint64_t> changes;
		changes.push_back(mod1);
		changes.push_back(mod2);

		SubscribeList::instance()->publish(changes);

	}

	return NULL;
}
