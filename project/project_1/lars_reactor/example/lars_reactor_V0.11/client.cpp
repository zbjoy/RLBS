#include "tcp_client.h"
#include <string.h>

//注册一个客户端处理服务器返回数据的回调业务
//typedef void msg_callback(const char *data, uint32_t len, int msgid, net_connection *conn, void *user_data);
void callback_busi(const char *data, uint32_t len, int msgid, net_connection* conn, void *user_data)
{
    //将数据写回去
    conn->send_message(data, len, msgid);
}


//打印业务
void print_busi(const char *data, uint32_t len, int msgid, net_connection *conn, void *user_data)
{
    printf("print busi is called!\n");
    printf("recv from server : [%s]\n", data);
    printf("msgid = %d\n", msgid);
    printf("len = %d\n", len);
}

//客户端创建连接之后hook业务
void on_client_build(net_connection *conn, void *args)
{
    printf("==> on_client build !\n");
    //客户端一旦连接成功 就会主动server发送一个msgid = 1 的消息
    int msgid = 1;
    const char *msg = "hello Lars";

    conn->send_message(msg, strlen(msg), msgid);
}

//客户端销毁连接之前的hook业务
void on_client_lost(net_connection *conn, void *args)
{
    printf("==> on_client lost !\n");
}

int main() 
{
    event_loop loop;

    tcp_client *client = new tcp_client(&loop, "127.0.0.1", 8888);

    client->add_msg_router(1, print_busi);
    client->add_msg_router(2, callback_busi);
    client->add_msg_router(200, print_busi);
    client->add_msg_router(404, print_busi);


    //注册hook函数
    client->set_conn_start(on_client_build);
    client->set_conn_close(on_client_lost);

    loop.event_process();
}
