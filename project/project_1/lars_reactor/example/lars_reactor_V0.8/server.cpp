#include "tcp_server.h" 
#include "string.h"

//定义一个回显的业务

//typedef void msg_callback(const char *data, uint32_t len, int msgid, net_connection* conn, void *user_data);
void callback_busi(const char *data, uint32_t len, int msgid, net_connection *conn, void *user_data)
{
    //直接将数据发回去
    //此时net_connection*conn--->  tcp_conn对象 ,conn就是能够跟对端客户端通信的tcp_conn链接对象
    conn->send_message(data, len, msgid);
}

//打印业务
void print_busi(const char *data, uint32_t len, int msgid, net_connection *conn, void *user_data)
{
    printf("recv from client : [%s]\n", data);
    printf("msgid = %d\n", msgid);
    printf("len = %d\n", len);
}

//新客户端创建成功之后的回调
void on_client_build(net_connection *conn, void *args)
{
    printf("===> on_client_build is called!\n");
    int msgid = 200;
    const char *msg = "welcome ! you are online!!!!";

    conn->send_message(msg, strlen(msg), msgid);
}

//客户端断开之前的回调
void on_client_lost(net_connection *conn, void *args)
{
    printf("===> on_client_lost is called!\n");
    printf("connection is lost !\n");
}



int main()
{
    event_loop loop;

    tcp_server server(&loop, "127.0.0.1", 7777);
    
    //注册一些回调方法
    server.add_msg_router(1, callback_busi);
    server.add_msg_router(2, print_busi);

    //注册链接hook函数
    server.set_conn_start(on_client_build);
    server.set_conn_close(on_client_lost);

    loop.event_process();

    return 0;
}
