#include "route_lb.h"


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
            //lb->update(rsp);
        }
    }
    
    //3 加入到load_balance的host_map集合中
    
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
        rsp.set_retcode(rlbs::RET_NOEXIST);

        ret = rlbs::RET_NOEXIST;
    } 
    
    pthread_mutex_unlock(&_mutex);
    
    return ret;
}


