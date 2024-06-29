#pragma once


//解决tcp粘包问题 的消息封装头
struct msg_head 
{
    int msgid;  //当前的消息类型
    int msglen; //消息体的长度
};


//消息头的长度，固定值
#define MESSAGE_HEAD_LEN 8

//消息头+消息体 最大长度限制
#define MESSAGE_LENGTH_LIMIT (65535 - MESSAGE_HEAD_LEN)



