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
#include "util/pthread_rwlock.h"

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
        bool StartCenter();

        void UpdateCenterStatus(CenterStatus cs);
        void UpdateOCStatus(const std::string& addr, CenterStatus cs);
        void UpdateLeadingCenter(const std::string& addr);

        /*
         * FastLeaderElection算法 选举出 RPC Center集群的Leader
         */
        bool FastLeaderElection();

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

        /*
         * 服务器启动时间，选举期间作为是否作为leader 标准之一
         */
        time_t start_time_;

        /*
         * 投票轮次，启动时候为最小，与其他Center服务器通信时候 以最大
         * 投票轮次为准
         */
        unsigned long logical_clock_;


        /*
         * 各种RWLock
         */
        RWLock status_rwlock_;
        RWLock oc_rwlock_;
        RWLock lc_rwlock_;


        DISALLOW_COPY_AND_ASSIGN(RpcCenter);
};


}  // end of namespace



#endif // __RPC_CENTER_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
