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

#include "center_proto/centers.pb.h"
#include "config_parser/config_parser.h"
#include "util/disallow_copy_and_assign.h"
#include "util/pthread_rwlock.h"

namespace libevrpc {

class RpcCenter;
struct OtherCenter {
    time_t start_time;
    CenterStatus center_status;
    char leader_center[128];
};

typedef std::shared_ptr<OtherCenter> OCPTR;
typedef std::unordered_map<std::string, OCPTR> HashMap;
typedef std::unordered_map<std::string, int32_t> CountMap;

class RpcCenter {
    public:
        ~RpcCenter();

        static RpcCenter& GetInstance(const std::string& config_file);

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
        CenterAction LeaderPredicate(const CentersProto& centers_proto);
        /*
         * 中心服务机器处理，服务于其他中心机器，RPC客户端，RPC集群
         */
        bool CenterProcessor(int32_t conn_fd);

    private:
        RpcCenter(const std::string& config_file);

    private:
        /*
         * 读取配置文件使用
         */
        ConfigParser& config_parser_instance_;
        /*
         * 当前Center机器的状态
         */
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
