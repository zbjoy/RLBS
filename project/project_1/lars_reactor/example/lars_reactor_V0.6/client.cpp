#include "tcp_client.h"

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

int main() 
{
    event_loop loop;

    tcp_client *client = new tcp_client(&loop, "127.0.0.1", 7777);

    client->add_msg_router(1, print_busi);
    client->add_msg_router(2, callback_busi);

    loop.event_process();
}
