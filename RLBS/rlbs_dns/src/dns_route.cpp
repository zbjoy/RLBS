// #include "mysql.h"
#include "dns_route.h"
#include "config_file.h"
#include <stdio.h>
#include <string.h>

using namespace std;

Route* Route::_instance = NULL;
pthread_once_t Route::_once = PTHREAD_ONCE_INIT;

Route::Route()
{
	printf("Route init\n");	

	// 初始化锁
	pthread_rwlock_init(&_map_lock, NULL);
	

	// 初始化 map
	_data_pointer = new route_map(); // RouteDataMap_A
	_temp_pointer = new route_map(); // RouteDataMap_B

	this->connect_db();

	// 将数据库中的 RouteData 的数据加载到 _data_poiner 中
	this->build_maps();

}

void Route::build_maps()
{
	int ret = 0;	
	char sql[1000];
	memset(sql, 0, 1000);

	// 查询 RouteData 数据库
	snprintf(sql, 1000, "SELECT * FROM RouteData");
	ret = mysql_real_query(&_db_conn, sql, strlen(sql));
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
