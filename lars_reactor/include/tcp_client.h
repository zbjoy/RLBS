#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "reactor_buf.h"
#include "event_loop.h"

class tcp_client;
typedef void msg_callback(const char *data, uint32_t len, int msgid, tcp_client *client, void *user_data);

class tcp_client
{
public:
    tcp_client(event_loop *loop, const char *ip, unsigned short port);

    //发送方法
    int send_message(const char *data, int msglen, int msgid);

    //处理读业务
    void do_read();

    //处理写业务
    void do_write();

    //释放链接
    void clean_conn();

    //链接服务器
    void do_connect();


    //设置业务处理的回调函数
    void set_msg_callback(msg_callback *msg_cb)
    {
        this->_msg_callback = msg_cb;
    }



    //输入缓冲
    input_buf ibuf;
    //输出缓冲
    output_buf obuf;
    //server端ip地址
    struct sockaddr_in _server_addr;
    socklen_t _addrlen;

private:
    int _sockfd; //当前客户端链接


    //客户端事件的处理机制
    event_loop *_loop;

    //客户端处理服务器消息的回调业务函数
    msg_callback *_msg_callback; 

};
