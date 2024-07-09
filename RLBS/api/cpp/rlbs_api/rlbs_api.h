#pragma once

#include "rlbs_reactor.h"
#include <string>

class rlbs_client
{
public:
	rlbs_client();
	~rlbs_client();

	int get_host(int modid, int cmdid, std::string& ip, int& port);

	void report(int modid, int cmdid, std::string& ip, int port, int retcode);

private:
	int _sockfd[3];
	uint32_t _seqid;

};
