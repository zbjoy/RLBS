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


    int ret = api.get_host(modid, cmdid, ip, port);
    if (ret == 0) {
        std::cout <<"host is " << ip << ":" << port << std::endl;

        //根据ip+port 做业务
        
        //上报ip+port 结果
        api.report(modid, cmdid, ip, port, 0);
    }
    else if (ret == 3) {//RET_NOEXSIT
        printf("not exsit!\n");
    }



    return 0;
}

