#pragma once

#include "rlbs_reactor.h"
#include "rlbs.pb.h"
#include <pthread.h>
#include "route_lb.h"

void start_UDP_servers(void);
void start_dns_client(void);
void start_report_client(void);

extern thread_queue<rlbs::ReportStatusRequest>* report_queue;
extern thread_queue<rlbs::GetRouteRequest>* dns_queue;
extern route_lb* r_lb[3];
