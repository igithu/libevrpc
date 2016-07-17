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

enum CenterAction {
    PROPOSAL,
    ACCEPT,
    REFUSED
};

class RpcCenter;
struct OtherCenter {
    time_t start_time;
    CenterStatus center_status;
    char leader_center[128];
};

typedef std::shared_ptr<OtherCenter> OCPTR;
typedef std::unordered_map<std::string, OCPTR> HashMap;
typedef std::unordered_map<std::string, int32_t> CountMap;


struct CenterData {
    CenterStatus center_status;
    CenterAction center_action;
    time_t start_time;
    time_t lc_start_time;
    unsigned long logical_clock;
    char leader_center[128];
};


class RpcCenter {
    public:
        ~RpcCenter();

        static RpcCenter& GetInstance();

        bool InitRpcCenter();
        bool StartCenter();

        void UpdateCenterStatus(CenterStatus cs);
        void UpdateOCStatus(const std::string& addr, OCPTR& oc_ptr);
        void UpdateLeadingCenter(const std::string& addr);
        void IncreaseLogicalClock();

        CenterStatus GetLocalCenterStatus();
        time_t GetOCStartTime(const std::string& leader_center);
        std::string GetLeadingCenter();
        unsigned long GetLogicalClock();

        /*
         * FastLeaderElection算法 选举出 RPC Center集群的Leader
         */
        bool FastLeaderElection(const char* recommend_center);
        /*
         * 判断新的Proposal数据进行预判，是否需要更新本地Leader信息
         */
        CenterAction LeaderPredicate(struct CenterData& center_data);

        /*
         * 收发Center数据
         */
        bool Receiver(int32_t fd, struct CenterData& cd);
        bool Sender(int32_t fd, const struct CenterData& cd);

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
         * 服务器启动时间，选举期间作为是否作为leader的标准之一
         */
        time_t start_time_;

        /*
         * 投票轮次，启动时候为最小，与其他Center服务器通信时候 以最大
         * 投票轮次为准
         */
        unsigned long logical_clock_;

        /*
         * Center服务器端口
         */
        char* center_port_;


        /*
         * 各种RWLock
         */
        RWLock status_rwlock_;
        RWLock oc_rwlock_;
        RWLock lc_rwlock_;
        RWLock logical_clock_rwlock_;


        DISALLOW_COPY_AND_ASSIGN(RpcCenter);
};


}  // end of namespace



#endif // __RPC_CENTER_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
