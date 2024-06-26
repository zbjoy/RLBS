#include "event_loop.h"
#include <stdio.h>



//构造 用于创建epoll
event_loop::event_loop()
{
    //创建一个epoll句柄 文件描述符
    _epfd = epoll_create1(0);
    if (_epfd == -1) {
        fprintf(stderr, "epoll create error\n");
        exit(1);
    }
}

//阻塞循环监听事件，并且处理 epoll_wait, 包含调用对应的触发回调函数
void event_loop::event_process()
{
    io_event_map_it ev_it;
    while (true) {

        printf("wait IN OUT event...\n");
        for (listen_fd_set::iterator it = listen_fds.begin(); it != listen_fds.end(); it++) {
            printf("fd %d is listenning by event_loop...\n", *it);
        }

        int nfds = epoll_wait(_epfd, _fired_evs, MAXEVENTS, -1);
        for (int i = 0; i < nfds; i ++) {
            //通过epoll触发的fd 从map中找到对应的io_event
            ev_it = _io_evs.find(_fired_evs[i].data.fd);

            //取出对应的事件
            io_event *ev = &(ev_it->second);

            if (_fired_evs[i].events & EPOLLIN) {
                //读事件， 调用读回调函数
                void *args = ev->rcb_args;
                //调用读业务
                ev->read_callback(this, _fired_evs[i].data.fd, args);
            }
            else if (_fired_evs[i].events & EPOLLOUT) {
                //写事件, 调用写回调函数
                void *args = ev->wcb_args;
                ev->write_callback(this, _fired_evs[i].data.fd, args);
            }
            else if (_fired_evs[i].events & (EPOLLHUP|EPOLLERR)) {
                //水平触发未处理， 可能会出现HUP事件，需要正常处理读写， 如果当前时间events 既没有写，也没有读 将events从epoll中删除
                if (ev->read_callback != NULL) {
                    //读事件， 调用读回调函数
                    void *args = ev->rcb_args;
                    //调用读业务
                    ev->read_callback(this, _fired_evs[i].data.fd, args);
                }
                else if (ev->write_callback != NULL) {
                    //写事件, 调用写回调函数
                    void *args = ev->wcb_args;
                    ev->write_callback(this, _fired_evs[i].data.fd, args);
                }
                else  {
                    //删除
                    fprintf(stderr, "fd %d get error， delete from epoll", _fired_evs[i].data.fd);
                    this->del_io_event(_fired_evs[i].data.fd);
                }
            }
        }
    }
}

//添加一个io事件到event_loop中 //添加和修改
void event_loop::add_io_event(int fd, io_callback *proc, int mask, void *args)
{ 
    int final_mask;
    int op;

    //1 找到当前的fd是否已经是已有事件
    io_event_map_it it = _io_evs.find(fd);
    if (it == _io_evs.end()) {
        // 如果没有事件， ADD方式添加到epoll中
        op = EPOLL_CTL_ADD;    
        final_mask = mask;
    }
    else {
        // 如果事件存在, MOD方式添加到epoll中
        op = EPOLL_CTL_MOD;
        final_mask = it->second.mask | mask;
    }
    
    //2 将fd 和 io_callback绑定 map中
    if (mask & EPOLLIN) {
        //该事件是一个读事件
        _io_evs[fd].read_callback = proc; //注册读回调业务
        _io_evs[fd].rcb_args = args;
    }
    else if (mask & EPOLLOUT) {
        //该事件是一个写事件
        _io_evs[fd].write_callback = proc; //注册写回调业务
        _io_evs[fd].wcb_args = args;
    }

    //****
    _io_evs[fd].mask = final_mask;
    
    //3 将当前的事件 加入到 原生的epoll
    struct epoll_event event;
    event.events = final_mask;
    event.data.fd = fd;
    if (epoll_ctl(_epfd, op, fd, &event) == -1) {
        fprintf(stderr, "epoll ctl %d error\n", fd);
        return;
    }
    
    //4 将当前fd加入到  正在监听的fd 集合中
    listen_fds.insert(fd);
}

//删除一个io事件 从event_loop中
void event_loop::del_io_event(int fd)
{
    io_event_map_it it = _io_evs.find(fd);
    if (it == _io_evs.end()) {
        return;
    }

    //将事件从_io_evs map中删除  
    _io_evs.erase(fd);

    //将fd 从 listen_fds set 中删除
    listen_fds.erase(fd);

    //将fd 从 原生的epoll堆中删除
    epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL);
}

//删除一个io事件的某个触发条件(EPOLLIN/EPOLLOUT)
void event_loop::del_io_event(int fd, int mask)
{
    io_event_map_it it = _io_evs.find(fd);
    if (it == _io_evs.end()) {
        return;
    }

    //此时it就是当前要删除的事件对 key:fd value:io_event
    int &o_mask = it->second.mask;
    o_mask = o_mask & (~mask);

    if (o_mask == 0) {
        //通过删除条件已经没有触发条件
        printf(" o_mask == 0 del io event \n");
        this->del_io_event(fd);
    }
    else {
        //如果事件还存在，则修改当前事件
        struct epoll_event event;
        event.events = o_mask;
        event.data.fd = fd;

        epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &event);
    }
}

