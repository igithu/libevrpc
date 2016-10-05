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

#include <map>
#include <memory>
#include <string>
#include <unordered_map>

#include "center_server_thread.h"
#include "election_thread.h"
#include "reporter_thread.h"
#include "leader_thread.h"
#include "rpc_center/load_balancer/load_balancer.h"
#include "center_proto/centers.pb.h"
#include "config_parser/config_parser.h"
#include "util/disallow_copy_and_assign.h"
#include "util/pthread_rwlock.h"

namespace libevrpc {

class RpcCenter;
struct OtherCenter {
    time_t start_time;
    CenterStatus center_status;
    uint32_t vote_count;
    char current_follow_leader_center[128];
};

struct LeaderInfos {
    /*
     * lc: leader center
     */
    time_t lc_start_time;
    std::string leader_center;
};


typedef std::shared_ptr<OtherCenter> OCPTR;
typedef std::unordered_map<std::string, OCPTR> HashMap;
typedef std::unordered_map<std::string, int32_t> CountMap;
typedef std::map<uint32_t, std::string> CENTER_HASH_MAP;

extern std::string g_config_file;

class RpcCenter {
    public:
        ~RpcCenter();

        static RpcCenter& GetInstance(const std::string& config_file);

        bool InitRpcCenter();
        bool StartCenter();
        bool StopCenter();
        void WaitCenter();

        void UpdateCenterStatus(CenterStatus cs);
        bool UpdateOCStatus(const CentersProto& centers_proto);
        void UpdateLeadingCenterInfos(const CentersProto& centers_proto);
        void IncreaseLogicalClock();

        CenterStatus GetLocalCenterStatus();
        time_t GetOCStartTime(const std::string& leader_center);
        std::string GetLeadingCenter();
        time_t GetLeadingCenterStartTime();
        unsigned long GetLogicalClock();
        CenterStatus GetCenterStatus();

        /*
         * FastLeaderElection算法 选举出 RPC Center集群的Leader
         */
        bool FastLeaderElection();
        /*
         * 群发信息，询问各个机器，获取各个Center机器的信息
         */
        bool InquiryCenters();
        /*
         * 发起Proposal
         */
        bool ProposalLeaderElection();
        /*
         * 判断新的Proposal数据进行预判，是否需要更新本地Leader信息
         */
        CenterAction LeaderPredicate(const CentersProto& centers_proto);
        /*
         * 中心服务机器处理，服务于其他中心机器，RPC客户端，RPC集群
         */
        bool CenterProcessor(int32_t conn_fd);

        bool ProcessCenterData(int32_t fd, const CentersProto& centers_proto);
        /*
         * 开始FastLeaderElection,同时启动Election线程
         */
        bool StartFastLeaderElection();

        /**
         * 标示判决FastLeaderElection是否在运行
         */
        void SetFastLeaderRunning(bool is_running);

        bool ReporterProcessor(int32_t conn_fd);

    private:
        RpcCenter(const std::string& config_file);

        bool BroadcastInfo(std::string& bc_info);

        bool IsFastLeaderRunning();

        bool RegistNewCenter(const std::string& new_center);

    private:
        /*
         * Center服务器统一端口
         */
        char* center_port_;
        /*
         * 读取配置文件使用
         */
        ConfigParser& config_parser_instance_;
        /*
         * 当前Center机器的状态,服务启动时间,当前LeaderCenter信息,
         */
        CenterStatus center_status_;
        time_t start_time_;
        LeaderInfos* leader_infos_ptr_;

        /*
         * 记录其他Center服务器状态
         */
        HashMap* other_centers_ptr_;

        /*
         * 选举生效票数
         */
        uint32_t election_done_num_;
        /*
         * 投票轮次，启动时候为最小，与其他Center服务器通信时候 以最大
         * 投票轮次为准
         */
        unsigned long logical_clock_;
        /*
         * FastLeaderElection算法是否在运行，主要防止多线程同时运行，运行两次FastLeaderElection
         */
        bool fastleader_election_running_;


        /*
         * 各种RWLock
         */
        RWLock status_rwlock_;
        RWLock oc_rwlock_;
        RWLock lc_rwlock_;
        RWLock logical_clock_rwlock_;
        RWLock fle_running_rwlock_;
        RWLock center_hash_map_rwlock_;

        /*
         * 各种线程
         */
        CenterServerThread* center_server_thread_;
        ElectionThread* election_thread_;
        ReporterThread* reporter_thread_;
        LeaderThread* leader_thread_;

        /*
         * 负载均衡插件
         */
        LoadBalancer* load_balancer_ptr_;

        /*
         * Center机器Hash表
         */
        CENTER_HASH_MAP* center_hash_map_ptr_;

        DISALLOW_COPY_AND_ASSIGN(RpcCenter);
};



}  // end of namespace



#endif // __RPC_CENTER_H



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
