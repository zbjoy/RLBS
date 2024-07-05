#include "tcp_client.h"

//注册一个客户端处理服务器返回数据的回调业务
//typedef void msg_callback(const char *data, uint32_t len, int msgid, tcp_client *client, void *user_data);
void busi(const char *data, uint32_t len, int msgid, tcp_client *client, void *user_data)
{
    printf("recv from server\n");
    printf("msgid = %d\n", msgid);
    printf("len = %d\n", len);
    printf("data = %s\n", data);
    printf("===========\n");

    client->send_message(data, len, msgid);
}

int main() 
{
    event_loop loop;

    tcp_client *client = new tcp_client(&loop, "127.0.0.1", 7777);

    //注册一个回调业务
    client->set_msg_callback(busi);

    loop.event_process();
}
