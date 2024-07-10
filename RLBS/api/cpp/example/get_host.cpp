#include "rlbs_api.h"
#include <iostream>

void usage()
{
    printf("usage : ./get_host [modid] [cmdid]\n");
}

// 开发者业务模块
int main(int argc, char** argv)
{
    if (argc != 3)
    {
        usage();
        return 1;
    }

    rlbs_client api;

    std::string ip;
    int port;

    int ret = 0;
    int modid = atoi(argv[1]);
    int cmdid = atoi(argv[2]);

    // 1 将 modid/cmdid 注册 (只调用一次)
    ret = api.reg_init(modid, cmdid);
    if (ret != 0)
    {
        std::cerr << "modid " << modid << ", cmdid " << cmdid << " not exist, register error ret = " << ret << std::endl; 
    }

    // 2 获取一个主机信息
    ret = api.get_host(modid, cmdid, ip, port);
    if (ret == 0)
    {
        std::cout << "ip = " << ip << " port = " << port << std::endl;
    }
    else if (ret == 3)
    {
        printf("not exist!\n");
    }
    
    return 0;
}