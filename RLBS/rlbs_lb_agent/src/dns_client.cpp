#include "main_server.h"



//dns service已经恢复当前modid/cmdid对应的全部的主机集合
void deal_recv_route(const char *data, uint32_t len, int msgid, net_connection *conn, void *user_data)
{
    rlbs::GetRouteResponse rsp;
    rsp.ParseFromArray(data, len);

    int modid = rsp.modid();
    int cmdid = rsp.cmdid();

    int index = (modid+cmdid) % 3;

    //将 modid/cmdid交给一个route_lb来处理，将rsp中的hostinfo集合加入到对应route_lb中 
    r_lb[index]->update_host(modid, cmdid, rsp);
}

//如果队列中有数据，所触发的一个回调业务
void new_dns_request(event_loop *loop, int fd, void *args)
{
    tcp_client *client = (tcp_client*)args;

    //1 将请求数据从dns_queue中取出
    std::queue<rlbs::GetRouteRequest>  msgs;
    
    //2 将取出的数据放在一个queue容器中
    dns_queue->recv(msgs);

    //3 遍历queue容器的元素， 依次将每个元素消息 发送给dns service
    while ( !msgs.empty() ) {
        rlbs::GetRouteRequest req = msgs.front();
        msgs.pop();

        std::string requestString;
        req.SerializeToString(&requestString);

        //将这个消息透传给dns service
        client->send_message(requestString.c_str(), requestString.size(), rlbs::ID_GetRouteRequest);
    }
}

void conn_init(net_connection* conn, void* args)
{
    for (int i = 0; i < 3; i++)
    {
        r_lb[i]->reset_lb_status();
    }
}


//dns client 线程的主业务
void *dns_client_thread(void *args)
{
    printf("dns client thread start!\n");
    event_loop loop;

    // config_file::setPath("./conf/lb_agent.ini");
    std::string ip = config_file::instance()->GetString("dns", "ip", "127.0.0.1");
    short port = config_file::instance()->GetNumber("dns", "port", 7778);

    //创建客户端
    tcp_client client(&loop, ip.c_str(), port);


    //将dns_queue绑定到loop中，让loop监控queue的数据
    dns_queue->set_loop(&loop);
    dns_queue->set_callback(new_dns_request, &client);


    //注册一个回调函数 用来处理dns server的返回的消息
    client.add_msg_router(rlbs::ID_GetRouteResponse, deal_recv_route);

    // 设置一个当前 dns client 的创建连接成功的 Hook 函数
    client.set_conn_start(conn_init);


    loop.event_process();

    return NULL;
}

void start_dns_client(void)
{
    //开辟一个线程
    pthread_t tid;

    int ret = pthread_create(&tid, NULL, dns_client_thread, NULL);
    if (ret == -1) {
        perror("pthread_create");
        exit(1);
    }

    pthread_detach(tid);
}

