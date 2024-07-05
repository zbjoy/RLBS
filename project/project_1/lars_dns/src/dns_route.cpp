#include "dns_route.h"
#include "config_file.h"
#include <stdio.h>
#include <string.h>


using namespace std;

//单例初始化
Route *Route::_instance = NULL;
pthread_once_t Route::_once = PTHREAD_ONCE_INIT;

Route::Route() {
    printf("Route init\n");

    //初始化锁
    pthread_rwlock_init(&_map_lock, NULL);

    //初始化 map
    _data_pointer = new route_map(); //RouteDataMap(A)
    _temp_pointer = new route_map(); //RouteDataMap(B)

    //链接数据库
    this->connect_db();


    //将数据库中的RouteData的数据加载到_data_pointer中
    this->build_maps(); 
}

void Route::build_maps() 
{
    int ret = 0;
    char sql[1000];
    memset(sql, 0, 1000);

    //查询RouteData数据库 
    //"Select * from RouteData;"
    snprintf(sql, 1000, "SELECT * FROM RouteData;");
    ret = mysql_real_query(&_db_conn, sql, strlen(sql));
    if (ret != 0) {
        fprintf(stderr, "select RouteData error %s\n", mysql_error(&_db_conn));
        exit(1);
    }
    
    //获得一个结果集合
    MYSQL_RES *result = mysql_store_result(&_db_conn);

    //得到行数
    long line_num = mysql_num_rows(result);
    
    //遍历分析集合中的元素， 加入_data_pointer中(MapA中)
    MYSQL_ROW row;
    for (int i = 0; i < line_num; i++) {
        //处理一行的数据
        row = mysql_fetch_row(result);
        int modID = atoi(row[1]);
        int cmdID = atoi(row[2]);
        unsigned int ip = atoi(row[3]);
        int port = atoi(row[4]);

        printf("modid = %d, cmdid = %d, ip = %u, port = %d\n", modID, cmdID, ip, port);

        //将读到数据加入map中
        //组装一个map的key
        uint64_t key = ((uint64_t)modID << 32) + cmdID;
        uint64_t value = ((uint64_t)ip << 32) + port;

        //插入到RouteDataMap_A中
        (*_data_pointer)[key].insert(value);
    }
}

//链接数据库的方法
void Route::connect_db()
{
    //加载mysql的配置参数
    string db_host = config_file::instance()->GetString("mysql", "db_host", "127.0.0.1");
    short db_port = config_file::instance()->GetNumber("mysql", "db_port", 3306);
    string db_user = config_file::instance()->GetString("mysql", "db_user", "root");
    string db_passwd = config_file::instance()->GetString("mysql", "db_passwd", "aceld");
    string db_name = config_file::instance()->GetString("mysql", "db_name", "lars_dns");

    //初始化mysql链接
    mysql_init(&_db_conn);

    //设置一个超时定期重连
    mysql_options(&_db_conn, MYSQL_OPT_CONNECT_TIMEOUT, "30");
    //开启mysql断开链接自动重连机制
    my_bool reconnect = 1;
    mysql_options(&_db_conn, MYSQL_OPT_RECONNECT, &reconnect);

    //链接数据库
    if (!mysql_real_connect(&_db_conn, db_host.c_str(), db_user.c_str(), db_passwd.c_str(), db_name.c_str(), db_port, NULL, 0)) {
        fprintf(stderr, "Failed to connect mysql\n");
        exit(1);
    }

    printf("connect db succ!\n");
}


//通过modid/cmdid获取全部的当前模块所挂载的host集合
host_set Route::get_hosts(int modid, int cmdid)
{
    host_set hosts;

    //组装key
    uint64_t key = ((uint64_t)modid << 32) + cmdid;


    //通过map来得到
    pthread_rwlock_rdlock(&_map_lock);
    route_map_it it = _data_pointer->find(key);
    if (it != _data_pointer->end()) {
        //找到了对应的host set
        hosts = it->second; 
    }

    pthread_rwlock_unlock(&_map_lock);

    return hosts;
}
