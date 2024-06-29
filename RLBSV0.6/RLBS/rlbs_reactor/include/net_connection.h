#pragma once

class net_connection
{
public:	
	net_connection() {}

	virtual int send_message(const char* data, int msglen, int msgid) = 0;
};
