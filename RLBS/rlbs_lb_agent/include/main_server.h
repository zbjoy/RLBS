#pragma once

#include "rlbs_reactor.h"
#include "rlbs.pb.h"
#include <pthread.h>
#include "route_lb.h"
#include "config_file.h"

void start_UDP_servers(void);
void start_dns_client(void);
void start_report_client(void);

extern thread_queue<rlbs::ReportStatusRequest>* report_queue;
extern thread_queue<rlbs::GetRouteRequest>* dns_queue;
extern route_lb* r_lb[3];

struct load_balance_config
{
	int probe_num;	

	int init_succ_cnt;

	int init_err_cnt;

	int err_rate;

	float succ_rate;

	// 当idle节点连续失败次数超过此值，节点变为overload状态
	int contin_err_limit;

    //当overload节点成功次数超过此值，节点变为idle状态
    int contin_succ_limit;

    //对于某个modid/cmdid下的idle状态的主机，需要清理一次负载窗口的时间
    int idle_timeout;

    //对于某个modid/cmdid下的overload状态主机， 在过载队列等待的最大时间
    int overload_timeout;

    //对于每个NEW状态的modid/cmdid 多久从远程dns更新一次到本地路由
    long update_timeout;	
};

extern struct load_balance_config lb_config;
