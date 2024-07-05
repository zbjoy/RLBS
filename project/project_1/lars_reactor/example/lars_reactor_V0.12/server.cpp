#include "tcp_server.h" 
#include "string.h"
#include "config_file.h"

/*
struct player
{
    net_connection *conn;//可以跟对应的客户端的玩家进行通信
};
*/

tcp_server *server;

//当前任务的一个task callback
void print_lars_task(event_loop* loop, void *args)
{
    printf("======== Active Task Func ===== \n");

    //得到当前thread的在线的fd有哪些
    listen_fd_set fds; 
    loop->get_listen_fds(fds);//从当前线程的loop获取，每个线程所监听的fd是不同的

    listen_fd_set::iterator it;  
    for (it = fds.begin(); it != fds.end(); it++) {
        int fd = *it; //当前线程已经建立连接并且监听中的在线的客户端
        
        tcp_conn *conn = tcp_server::conns[fd];//取出连接
        if (conn != NULL)  {
            int msgid = 404;        
            const char *msg = "Hello I am a Task!";
            conn->send_message(msg, strlen(msg), msgid);
        }
    }
}

//定义一个回显的业务
void callback_busi(const char *data, uint32_t len, int msgid, net_connection *conn, void *user_data)
{
    //直接将数据发回去
    //此时net_connection*conn--->  tcp_conn对象 ,conn就是能够跟对端客户端通信的tcp_conn链接对象
    conn->send_message(data, len, msgid);

    printf("conn param = %s\n", (char*)conn->param);
}

//打印业务
void print_busi(const char *data, uint32_t len, int msgid, net_connection *conn, void *user_data)
{
    printf("recv from client : [%s]\n", data);
    printf("msgid = %d\n", msgid);
    printf("len = %d\n", len);

    /*
    //如何知道是哪个玩家的发消息？
    struct player *player = (struct player*)conn->param;
    player.id =
    */
}

//新客户端创建成功之后的回调
void on_client_build(net_connection *conn, void *args)
{
    printf("===> on_client_build is called!\n");
    int msgid = 200;
    const char *msg = "welcome ! you are online!!!!";

    conn->send_message(msg, strlen(msg), msgid);


    //每次客户端在创建连接成功之后，执行一个任务
    server->get_th_pool()->send_task(print_lars_task);    

    /*
    //创建一个玩家对象
    struct player *player = new Player;
    player.conn = conn;
    int playId = 1;//累加器累加得到
    player.playerId = playId;

    conn->param = player;
    */

    //给当前的conn绑定一个自定义的参数，供之后来使用
    const char *conn_param_test = "I am the conn param for you!";
    conn->param = (void*)conn_param_test;
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

    //--->加载配置文件
    config_file::setPath("./reactor.ini");
    std::string ip = config_file::instance()->GetString("reactor", "ip", "0.0.0.0");
    short port = config_file::instance()->GetNumber("reactor", "port", 8888);


    server = new tcp_server(&loop, ip.c_str(), port);
    
    //注册一些回调方法
    server->add_msg_router(1, callback_busi);
    server->add_msg_router(2, print_busi);

    //注册链接hook函数
    server->set_conn_start(on_client_build);
    server->set_conn_close(on_client_lost);

    loop.event_process();

    return 0;
}
