#include "udp_client.h"
#include <string.h>

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

    udp_client *client = new udp_client(&loop, "127.0.0.1", 7777);

    //注册一个回调
    client->add_msg_router(1, print_busi);

    //udp client 主动给服务器发送一个消息
    int msgid = 1;
    const char *msg = "hello I am Lars udp client!";

    client->send_message(msg, strlen(msg), msgid);


    loop.event_process();
}
