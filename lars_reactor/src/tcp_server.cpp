#include <iostream>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <strings.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "tcp_server.h"
#include "tcp_conn.h"
#include "reactor_buf.h"

#if 0
void server_rd_callback(event_loop *loop, int fd, void *args);
void server_wt_callback(event_loop *loop, int fd, void *args);
#endif

void lars_hello()
{
    std::cout <<"lars Hello" <<std::endl;
}


#if 0
//临时的收发消息结构
struct message
{
    char data[m4K];
    char len;
};
struct message msg;
#endif

//typedef void io_callback(event_loop *loop, int fd, void *args);
void accept_callback(event_loop *loop, int fd, void *args)
{
    tcp_server *server = (tcp_server*)args;
    server->do_accept();
}




//构造函数
tcp_server::tcp_server(event_loop *loop, const char *ip, uint16_t port)
{
    //0. 忽略一些信号  SIGHUP, SIGPIPE
    if (signal(SIGHUP, SIG_IGN) == SIG_ERR)  {
        fprintf(stderr, "signal ignore SIGHUB\n");
    }

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)  {
        fprintf(stderr, "signal ignore SIGHUB\n");
    }
    
    //1. 创建socket
    _sockfd = socket(AF_INET, SOCK_STREAM |SOCK_CLOEXEC, IPPROTO_TCP) ;
    if (_sockfd == -1) {
        fprintf(stderr, "tcp::server :socket()\n");
        exit(1);
    }



    //2 初始化服务器的地址
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_aton(ip, &server_addr.sin_addr);
    server_addr.sin_port = htons(port);


    //2.5 设置sock可以重复监听
    int op = 1;
    if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op)) < 0) {
        fprintf(stderr, "set socketopt reuse error\n");
    }
    
    //3 绑定端口
    if (bind(_sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "bind error\n");
        exit(1);
    }
    
    //4 监听
    if (listen(_sockfd, 500) == -1) {
        fprintf(stderr , "listen error\n");
        exit(1);
    }

    //5 将形参loop添加到 tcp_server _loop中
    _loop = loop;

    //6 注册_sockfd 读事件---> accept处理
    _loop->add_io_event(_sockfd, accept_callback, EPOLLIN, this);
}

//开始提供创建链接的服务
void tcp_server::do_accept()
{
    int connfd;
    while (true) {
        //1 accept
        connfd = accept(_sockfd, (struct sockaddr*)&_connaddr, &_addrlen);
        if (connfd == -1) {
            if (errno == EINTR)  {
                //中断错误
                fprintf(stderr, "accept errno = EINTR\n");
                continue;
            }
            else if (errno == EAGAIN) {
                fprintf(stderr, "accept errno = EAGAIN\n");
                break;
            }
            else if (errno == EMFILE) {
                //建立链接过多， 资源不够
                fprintf(stderr, "accept errno = EMFILE\n");
                continue;
            }
            else {
                fprintf(stderr, "accept error");
                exit(1);
            }
        }
        else {
            //TODO 添加一些心跳机制
            //TODO 添加消息队列机制
            //
            //accept succ!
            printf("accept succ!\n");

            //创建一个新的tcp_conn连接对象
            tcp_conn *conn = new tcp_conn(connfd, _loop);
            if (conn == NULL) {
                fprintf(stderr, "new tcp_conn error\n");
                exit(1);
            }

            printf("get new connection succ!\n");
            break;
        }
    }
}

//析构函数  资源的释放
tcp_server::~tcp_server()
{
    close(_sockfd);
}

#if 0
//客户端connfd 注册的写事件回调业务
void server_wt_callback(event_loop *loop, int fd, void *args)
{
    struct message *msg = (struct message*) args;

    output_buf obuf;

    //将msg ---> obuf
    obuf.send_data(msg->data, msg->len);
    while (obuf.length()) {
        int write_num = obuf.write2fd(fd);
        if (write_num == -1) {
            fprintf(stderr, "write connfd error\n");
            return;
        }
        else if (write_num == 0) {
            //当前不可写
            break;
        }
    }

    //删除写事件， 添加读事件
    loop->del_io_event(fd, EPOLLOUT);
    loop->add_io_event(fd, server_rd_callback, EPOLLIN, msg);
}

//客户端connfd 注册的读事件的回调业务
void server_rd_callback(event_loop *loop, int fd, void *args)
{
    struct message *msg = (struct message *)args;
    int ret = 0;

    input_buf ibuf;

    printf("begin read...\n");
    ret = ibuf.read_data(fd);
    if (ret == -1) {
        fprintf(stderr, "ibuf read_data error\n");
        //当前的读事件删除
        loop->del_io_event(fd);

        //关闭对端fd
        close(fd);
        return ;
    }

    if (ret == 0) {
        //对方正常关闭
        //当前的读事件删除
        loop->del_io_event(fd);

        //关闭对端fd
        close(fd);

        return ;
    }
    printf("server rd callback is called!\n");


    //将读到的数据拷贝到msg中
    msg->len = ibuf.length();
    bzero(msg->data, msg->len);
    memcpy(msg->data, ibuf.data(), msg->len);

    ibuf.pop(msg->len);
    ibuf.adjust();


    printf(" recv data = %s\n", msg->data);


    //针对connfd  删除读事件， 添加写事件
    loop->del_io_event(fd, EPOLLIN);
    loop->add_io_event(fd, server_wt_callback, EPOLLOUT, msg);//epoll_wait会立刻触发EPOLLOUT事件
}
#endif
