/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file rpc_center.h
 * @author aishuyu(asy5178@163.com)
 * @date 2016/07/12 20:33:03
 * @brief
 *
 **/




#ifndef __RPC_CENTER_H
#define __RPC_CENTER_H

#include <string>
#include <unordered_map>

#include "util/disallow_copy_and_assign.h"

namespace libevrpc {

/*
 * 选举算法使用，决定哪一台机器为Leader，便于同步全局RpcServer列表
 */
enum CenterStatus {
    LOOKING,
    FOLLOWING,
    OBSERVING,
    LEADING,
    UNKONW,
    DEAD;
};

class RpcCenter;
struct OtherCenter {
    std::string ip_addr_;
    CenterStatus center_status;
};

typedef std::unordered_map<std::string, CenterStatus> HashMap;

class RpcCenter {
    public:
        ~RpcCenter();

        static RpcCenter& GetInstance();

        bool InitRpcCenter();

    private:
        RpcCenter();

    private:
        CenterStatus center_status_;
        /*
         * 记录其他Center服务器状态
         */
        HashMap* other_centers_ptr_;

        /*
         * 记录当前Leader机器
         */
        std::string leader_center_;


        DISALLOW_COPY_AND_ASSIGN(RpcCenter);
};


}  // end of namespace



#endif // __RPC_CENTER_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
