#pragma once

#include <netinet/in.h>
#include "event_loop.h"

class tcp_server
{
public:
    //构造函数
    tcp_server(event_loop * loop, const char *ip, uint16_t port);

    //开始提供创建链接的服务
    void do_accept();

    //析构函数  资源的释放
    ~tcp_server();

private:
    int _sockfd; //套接字 listen fd
    struct sockaddr_in  _connaddr; //客户端链接地址
    socklen_t _addrlen; //客户端链接地址长度


    //event_loop epoll的多路IO复用机制
    event_loop *_loop;
};


