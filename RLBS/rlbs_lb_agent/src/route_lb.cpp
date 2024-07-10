#include "route_lb.h"
#include "main_server.h"


route_lb::route_lb(int id)
{
    this->_id = id;
    pthread_mutex_init(&_mutex, NULL);
}

//将rsp中的hostinfo集合加入到对应route_lb中
int route_lb::update_host(int modid, int cmdid, rlbs::GetRouteResponse &rsp)
{
    //1 得到mod模块
    uint64_t mod = ((uint64_t)modid << 32)  + cmdid;
    
    pthread_mutex_lock(&_mutex);

    //2 找到对应的load_balance集合
    if (_route_lb_map.find(mod) != _route_lb_map.end()) {

        load_balance *lb = _route_lb_map[mod];

        if (rsp.host_size() == 0) {
            //dns返回的结果host主机目前是不存在
            delete lb;
            _route_lb_map.erase(mod);
        }
        else {
            //将rsp中的主机信息加入到load_balance当中
			//3 加入到load_balance的host_map集合中
            lb->update(rsp);
        }
    }
    
    
    pthread_mutex_unlock(&_mutex);

    return 0;
}

//agent 获取一个host主机 将返回的主机结果存放在rsp中
int route_lb::get_host(int modid, int cmdid, rlbs::GetHostResponse &rsp)
{
    int ret = rlbs::RET_SUCC;

    //1 将modid/cmdid--->key 
    uint64_t mod = ((uint64_t)modid << 32) + cmdid;

    pthread_mutex_lock(&_mutex);
    
    //2 从 _route_lb_map 去寻找key当前模块是否存在
    if (_route_lb_map.find(mod) != _route_lb_map.end()) {
        //TODO 3 如果 key是存在的
        //  通过load_balance获取key模块所可用的host信息
		load_balance* lb = _route_lb_map[mod];

		if (lb->empty() == true)
		{
			rsp.set_retcode(rlbs::RET_NOEXIST);
		}
		else
		{
			ret = lb->choice_one_host(rsp);

			rsp.set_retcode(ret);

            //触发的超时拉取机制
            if (lb->status == load_balance::NEW && time(NULL) - lb->last_update_time > lb_config.update_timeout) {
                lb->pull();
            }
		}
        
    }
    else {
        //4 如果 key不存在
        //  创建当前的modid/cmdid key 一条map数据 让load_balace向dns获取最新的host主机(modid/cmdid和host对应关系)信息
        
        //4.1 新建一个load_balance模块
        load_balance *lb = new load_balance(modid, cmdid);

        //4.2 将load_balance加入到 map中
        _route_lb_map[mod] = lb;
        
        //4.3 让load_balance从 dns service拉去当前mod模块所对应的host主机信息集合 
        lb->pull();

        //4.4 设置rsp中的回执的retcode 为不存在
		// fprintf(stderr, "route_lb.cpp: dns service 中 不存在\n");
        rsp.set_retcode(rlbs::RET_NOEXIST);

        ret = rlbs::RET_NOEXIST;
    } 
    
    pthread_mutex_unlock(&_mutex);
    
    return ret;
}

int route_lb::get_route(int modid, int cmdid, rlbs::GetRouteResponse& rsp)
{
    int ret = rlbs::RET_SUCC;

    // 1 将 modid/cmdid --> key
    uint64_t mod = ((uint64_t)modid << 32) + cmdid;

    pthread_mutex_lock(&_mutex);

    // 2 从 _route_lb_map 去寻找 key 当前模块是否存在
    if (_route_lb_map.find(mod) != _route_lb_map.end())
    {
        // 3 如果 mod 存在
        //   通过 load_balance 获取 key 模块所有可用的 host 信息
        load_balance* lb = _route_lb_map[mod];

        // 说明 lb 模块下有 host 主机信息
        std::vector<host_info*> vec;
        lb->get_all_hosts(vec);

        for (std::vector<host_info*>::iterator it = vec.begin(); it != vec.end(); it++)
        {
            rlbs::HostInfo host;
            host.set_ip((*it)->ip);
            host.set_port((*it)->port);
            rsp.add_host()->CopyFrom(host);
        }

        // 触发的超时拉取机制
        if (lb->status == load_balance::NEW && time(NULL) - lb->last_update_time > lb_config.update_timeout)
        {
            lb->pull();
        }

    }
    else
    {
        // 4 如果 mod 不存在

        // 4.1 新建一个 load_balance 模块
        load_balance* lb = new load_balance(modid, cmdid);

        // 4.2 将 load_balance 加入到 map 中
        _route_lb_map[mod] = lb;

        // 4.3 让 laod_balance 从 dns service 拉取当前 mod 模块对应的 host 主机信息集合
        lb->pull();

        ret = rlbs::RET_NOEXIST;
    }

    pthread_mutex_unlock(&_mutex);

    return ret;
}

//agent上报某个主机的结果
void route_lb::report_host(rlbs::ReportRequest &req)
{
    //1 通过请求中的模块mod 找到对应的load_balance负载均衡模块
    int modid = req.modid();  
    int cmdid = req.cmdid();
    int retcode = req.retcode();
    int ip = req.host().ip();
    int port = req.host().port();

    uint64_t mod = ((uint64_t)modid << 32) + cmdid;

    pthread_mutex_lock(&_mutex);
    if (_route_lb_map.find(mod) != _route_lb_map.end()) {
        //2 load_balance处理当前的上报请求
        load_balance *lb = _route_lb_map[mod];
        
        //2.1 通过对当前主机的上报的结果， 调整内部节点idle 和overload关系
        lb->report(ip, port, retcode);
        
        //2.2 再将最终的结果 再上报给 reporter service
        lb->commit();
    }

    pthread_mutex_unlock(&_mutex);
}


//将全部的load_balance都重置为NEW状态
void route_lb::reset_lb_status()
{
    pthread_mutex_lock(&_mutex);
    for (route_map_it it = _route_lb_map.begin(); it != _route_lb_map.end(); it++) {
        load_balance *lb = it->second;

        lb->status = load_balance::NEW;
    }
    pthread_mutex_unlock(&_mutex);
}

