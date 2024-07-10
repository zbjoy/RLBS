#include "rlbs_api.h"
#include <iostream>

void usage()
{
    printf("usage : ./example [modid] [cmdid]\n");
}

//开发者业务模块
int main(int argc, char **argv)
{
    if (argc != 3) {
        usage();
        return 1;
    }

    rlbs_client api;

    std::string ip; 
    int port;

    
    int modid = atoi(argv[1]);
    int cmdid = atoi(argv[2]);

    // 1 将 modid/cmdid 注册 (只调用一次)
    int ret = api.reg_init(modid, cmdid);
    if (ret != 0)
    {
        std::cout << "modid " << modid << "cmdid " << cmdid << " not exist, register error ret = " << ret << std::endl;
    }

    // 2 测试获取主机
    ret = api.get_host(modid, cmdid, ip, port);
    if (ret == 0)
    {
        std::cout << "host is " << ip << " : " << port << std::endl;

        // 上报 ip + port 结果
        api.report(modid, cmdid, ip, port, 0);
    }
    else if (ret == 3) // RET_NOEXIST
    {
        fprintf(stderr, "not exist!!\n");
    }

    // 3 获取全部的 host 信息
    route_set route;
    ret = api.get_route(modid, cmdid, route);
    if (ret == 0)
    {
        for (route_set_it it = route.begin(); it != route.end(); it++)
        {
            std::cout << "ip = " << (*it).first << ", port = " << (*it).second << std::endl;
        }
    }



    return 0;
}

