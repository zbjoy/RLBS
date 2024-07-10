#include "rlbs_api.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include "rlbs.pb.h"

rlbs_client::rlbs_client()
{
	printf("rlbs_client()\n");
	_seqid = 0;

	// 1 初始化服务器地址
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;

	// 默认的 ip 地址是本地, api 和 agent 应该部署在同一台设备上
	inet_aton("127.0.0.1", &servaddr.sin_addr);

	// 2 创建 3 个 UDP server
	for (int i = 0; i < 3; i++)
	{
		_sockfd[i] = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
		if (_sockfd[i] == -1)
		{
			perror("socket()");
			exit(1);
		}

		// 本连接对应 udp server 的端口号
		servaddr.sin_port = htons(8888 + i);

		// 连接
		int ret = connect(_sockfd[i], (const struct sockaddr*)&servaddr, sizeof(servaddr));
		if (ret == -1)
		{
			perror("cpnnect()\n");
			exit(1);
		}
	}
}

rlbs_client::~rlbs_client()
{
	for (int i = 0; i < 3; i++)
	{
		close(_sockfd[i]);
	}
}

int rlbs_client::get_host(int modid, int cmdid, std::string& ip, int& port)
{
	uint32_t seq = _seqid++;

	// 1 封装请求的 protobuf 消息
	rlbs::GetHostRequest req;
	req.set_seq(seq);
	req.set_modid(modid);
	req.set_cmdid(cmdid);

	// 2 打包成 rlbs 能够识别的 message
	char write_buf[4096], read_buf[20 * 4096];
	
	// 消息头
	msg_head head;
	head.msglen = req.ByteSizeLong();
	head.msgid = rlbs::ID_GetHostRequest;
	memcpy(write_buf, &head, MESSAGE_HEAD_LEN);
	req.SerializeToArray(write_buf + MESSAGE_HEAD_LEN, head.msglen);

	// 3 发送
	int index = (modid + cmdid) % 3;
	int ret = sendto(_sockfd[index], write_buf, head.msglen + MESSAGE_HEAD_LEN, 0, NULL, 0);
	if (ret == -1)
	{
		perror("send to");
		return rlbs::RET_SYSTEM_ERROR;
	}

	// 4 阻塞等待接收数据
	int message_len;
	rlbs::GetHostResponse rsp;

	do
	{
		message_len = recvfrom(_sockfd[index], read_buf, sizeof(read_buf), 0, NULL, 0);
		if (message_len == -1)
		{
			perror("recvfrom");
			return rlbs::RET_SYSTEM_ERROR;
		}

		memcpy(&head, read_buf, MESSAGE_HEAD_LEN);
		if (head.msgid != rlbs::ID_GetHostResponse)
		{
			fprintf(stderr, "message ID error\n");
			return rlbs::RET_SYSTEM_ERROR;
		}

		// 消息体
		ret = rsp.ParseFromArray(read_buf + MESSAGE_HEAD_LEN, message_len - MESSAGE_HEAD_LEN);
		if (!ret)
		{
			fprintf(stderr, "message format error\n");
			return rlbs::RET_SYSTEM_ERROR;
		}
	} while (rsp.seq() < seq);

	if (rsp.seq() != seq || rsp.modid() != modid || rsp.cmdid() != cmdid)
	{
		fprintf(stderr, "message format error\n");
		return rlbs::RET_SYSTEM_ERROR;
	}

	// 5 处理消息
	if (rsp.retcode() == rlbs::RET_SUCC)
	{
		rlbs::HostInfo host = rsp.host();	

		struct in_addr inaddr;
		inaddr.s_addr = host.ip();
		ip = inet_ntoa(inaddr);
		port = host.port();
	}

	return rsp.retcode();
}

void rlbs_client::report(int modid, int cmdid, std::string &ip, int port, int retcode)
{
	rlbs::ReportRequest req;

	req.set_modid(modid);
	req.set_cmdid(cmdid);
	req.set_retcode(retcode);

	rlbs::HostInfo* hp = req.mutable_host();

	struct in_addr inaddr;
	inet_aton(ip.c_str(), &inaddr);
	int ip_num = inaddr.s_addr;
	hp->set_ip(ip_num);
	hp->set_port(port);

	char write_buf[4096];

	msg_head head;
	head.msglen = req.ByteSizeLong();
	head.msgid = rlbs::ID_ReportRequest;
	memcpy(write_buf, &head, MESSAGE_HEAD_LEN);

	req.SerializeToArray(write_buf + MESSAGE_HEAD_LEN, head.msglen);

	int index = (modid+cmdid)%3;
	int ret = sendto(_sockfd[index], write_buf, MESSAGE_HEAD_LEN+head.msglen, 0, NULL, 0);
	if (ret == -1) {
		perror("sendto");
	}
}

int rlbs_client::get_route(int modid, int cmdid, route_set& route)
{
	// 1 封装请求的 protobuf 消息
	rlbs::GetRouteRequest req;
	req.set_modid(modid);
	req.set_cmdid(cmdid);

	// 2 打包成 rlbs 能够识别的 message
	char write_buf[4096], read_buf[20 * 4096];
	// 消息头

msg_head head;
    head.msglen = req.ByteSizeLong();
    head.msgid = rlbs::ID_API_GetRouteRequest;

    memcpy(write_buf, &head, MESSAGE_HEAD_LEN);
    req.SerializeToArray(write_buf + MESSAGE_HEAD_LEN, head.msglen);


    //3 发送
    int index = (modid + cmdid) %3;
    int ret = sendto(_sockfd[index], write_buf, head.msglen + MESSAGE_HEAD_LEN, 0, NULL, 0);
    if (ret == -1) {
        perror("send to");
        return rlbs::RET_SYSTEM_ERROR;
    }
    
    //4 阻塞等待接收数据
    int message_len;
    rlbs::GetRouteResponse rsp;

    message_len = recvfrom(_sockfd[index], read_buf, sizeof(read_buf), 0, NULL, 0);
    if (message_len == -1) {
        perror("recvfrom ");
        return rlbs::RET_SYSTEM_ERROR;
    }

    //消息头
    memcpy(&head, read_buf, MESSAGE_HEAD_LEN);
    if (head.msgid != rlbs::ID_API_GetRouteResponse) {
        fprintf(stderr, "message IDerrror\n");
        return rlbs::RET_SYSTEM_ERROR;
    }
    
    //消息体
    ret = rsp.ParseFromArray(read_buf + MESSAGE_HEAD_LEN, message_len-MESSAGE_HEAD_LEN);
    if (!ret) {
        fprintf(stderr, "message format error\n");
        return rlbs::RET_SYSTEM_ERROR;
    }



    if (rsp.modid() != modid || rsp.cmdid() != cmdid) {
        fprintf(stderr, "message format error\n");
        return rlbs::RET_SYSTEM_ERROR;
    }
    
    //5 处理消息
    for (int i = 0; i < rsp.host_size(); i++) {
        const rlbs::HostInfo &host = rsp.host(i);
        //将ip的网络字节序转换成主机字节序
        uint32_t host_ip = ntohl(host.ip()); 

        // ip 
        struct in_addr inaddr;
        inaddr.s_addr = host_ip;
        std::string ip = inet_ntoa(inaddr);

        // port
        int port = host.port();
        
        // ip+port --> route
        route.push_back(ip_port(ip, port));
    }
    
    return rlbs::RET_SUCC;
}


//注册一个模块
int rlbs_client::reg_init(int modid, int cmdid)
{
    route_set route;

    int retry_cnt = 0;

    while (route.empty() && retry_cnt < 3) {

        get_route(modid, cmdid, route);

        if (route.empty() == true) {
            usleep(50000); //等待50ms
        }
        else {
            break;
        }

        ++retry_cnt;
    }


    if (route.empty() == true) {
        return rlbs::RET_NOEXIST;//3
    }

    return rlbs::RET_SUCC; //0
}
