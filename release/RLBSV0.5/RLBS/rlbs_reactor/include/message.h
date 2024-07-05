#pragma once

struct msg_head
{
	int msgid;
	int msglen;
};

#define MESSAGE_HEAD_LEN 8

#define MESSAGE_LENGTH_LIMIT (65535 - MESSAGE_HEAD_LEN)
