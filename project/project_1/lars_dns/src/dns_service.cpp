#include "lars_reactor.h"
#include "dns_route.h"
#include "lars.pb.h"


//处理agent发送Route信息获取的业务
void get_route(const char *data, uint32_t len, int msgid, net_connection *conn, void *user_data)
{
    //1 解析proto文件
    lars::GetRouteRequest req;
    req.ParseFromArray(data, len);
    
    //2 得到 modID 和 cmdID
    int modid, cmdid;
    modid = req.modid();
    cmdid = req.cmdid();
    
    //3 通过modid/cmdid 获取host信息 从 _data_pointer所指向map中
    host_set hosts = Route::instance()->get_hosts(modid, cmdid);
    
    //4 打包一个新的response protobuf数据
    lars::GetRouteResponse rsp;
    rsp.set_modid(modid);
    rsp.set_cmdid(cmdid);

    for (host_set_it it = hosts.begin(); it != hosts.end(); it++) {
        //it就是set中的一个元素 ip+port 64位一个整形键值对
        uint64_t ip_port = *it;

        lars::HostInfo host;
        host.set_ip((uint32_t)(ip_port>>32));
        host.set_port((uint32_t)(ip_port));

        //将host添加到rsp 对象中
        rsp.add_host()->CopyFrom(host);
    }
    
    //5 发送给对方
    std::string responseString;
    rsp.SerializeToString(&responseString);
    conn->send_message(responseString.c_str(), responseString.size(), lars::ID_GetRouteResponse);
}

int main()
{
    event_loop loop;

    //--->加载配置文件
    config_file::setPath("./conf/lars_dns.ini");
    std::string ip = config_file::instance()->GetString("reactor", "ip", "0.0.0.0");
    short port = config_file::instance()->GetNumber("reactor", "port", 8888);


    tcp_server *server = new tcp_server(&loop, ip.c_str(), port);

    //注册一个回调业务
    server->add_msg_router(lars::ID_GetRouteRequest, get_route);
    


    loop.event_process();

    return 0;
}
