/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file rpc_center.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2016/07/12 20:37:30
 * @brief
 *
 **/


#include "rpc_center.h"

#include <io.h>
#include <time.h>
#include <fstream>
#include <iostream>

#include "util/rpc_communication.h"
#include "util/rpc_util.h"


namespace libevrpc {

using std::string;

RpcCenter::RpcCenter(const string& config_file) :
    config_parser_instance_(ConfigParser::GetInstance(config_file)),
    center_status_(LOOKING),
    other_centers_ptr_(new HashMap()),
    election_done_num_(0),
    start_time_(time(0)),
    logical_clock_(0),
    center_server_thread_(NULL),
    election_thread_(NULL),
    reporter_thread_(NULL) {
}

RpcCenter::~RpcCenter() {
    if (NULL != other_centers_ptr_) {
        delete other_centers_ptr_;
    }
    if (center_server_thread_ != NULL) {
        delete center_server_thread_;
    }
    if (election_thread_ != NULL) {
        delete election_thread_;
    }
    if (reporter_thread_ != NULL) {
        delete reporter_thread_;
    }
}

RpcCenter& RpcCenter::GetInstance() {
    static RpcCenter rc_instance;
    return rc_instance;
}

bool RpcCenter::InitRpcCenter() {
    const string cfile = "/tmp/centers.data";
    int ftyp = _access(cfile.c_str(), 0);
    if (0 != ftyp) {
        /*
         * 需要的服务器列表文件不存在，无法初始化后与其他机器进行通信
         * 启动失败!!!
         */
        return false;
    }

    const char* local_addr = GetLocalAddress();
    std::ifstream in(cfile);
    string line;
    while (getline (in, line)) {
        if (strcmp(line.c_str(), local_addr) == 0) {
            /*
             * 本地服务器信息忽略
             */
            continue;
        }
        /*
         * 初始中心服务信息
         */
        OCPTR oc_ptr = new OtherCenter();
        oc_ptr->vote_count = 0;
        memet(oc_ptr, 0, sizeof(OtherCenter));
        other_centers_ptr_->put(line, oc_ptr);
    }

    election_done_num_ = (other_centers_ptr_->size() + 1) / 2 + 1;

    /*
     * 初始化线程指针
     */
    center_server_thread_ = new CenterServerThread();
    election_thread_ = new ElectionThread();
    reporter_thread_ = new ReporterThread();


    return true;
}

bool RpcCenter::StartCenter() {
    if (!InitRpcCenter()) {
        return false;
    }

    if (!ProposalLeaderElection()) {
        fprintf(stderr, "Run the Ecletion failed!\n");
    }

    return true;
}

void RpcCenter::UpdateCenterStatus(CenterStatus cs) {
    WriteLockGuard wguard(status_rwlock_);
    center_status_ = cs;
}

bool RpcCenter::UpdateOCStatus(const CentersProto& centers_proto) {
    if (NULL == other_centers_ptr_) {
        fprintf(stderr, "Center error！ Other center info hasn't inited!\n");
        return false;
    }

    if (centers_proto.logical_clock() < GetLogicalClock()) {
        return false;
    }

    const string& addr = centers_proto.from_center_addr();
    {
        WriteLockGuard wguard(oc_rwlock_);
        /*
         * 更新当前中心服务器状态信息
         */
        HashMap::iterator iter = other_centers_ptr_->find(addr);
        if (iter == other_centers_ptr_->end()) {
            OCPTR oc_ptr = new OtherCenter();
            oc_ptr->start_time = centers_proto.start_time();
            oc_ptr->center_status = centers_proto.center_status();
            oc_ptr->vote_count = 0;
            strcpy(oc_ptr->current_follow_leader_center, centers_proto.leader_center().c_str())
            other_centers_ptr_->insert(std::make_pair(addr, oc_ptr));
        } else {
            OCPTR& oc_ptr = iter->second;
            oc_ptr->start_time = centers_proto.start_time();
            oc_ptr->center_status = centers_proto.center_status();
            strcpy(oc_ptr->current_follow_leader_center, centers_proto.leader_center().c_str())
        }
    }

    uint32_t vote_count = 0
    {
        WriteLockGuard wguard(oc_rwlock_);
        /*
         * 更新当前中心服务器投票的中心服务器信息
         */
        HashMap::iterator leader_iter = other_centers_ptr_->find(centers_proto.leader_center());
        if (leader_iter == other_centers_ptr_->end()) {
            OCPTR oc_ptr = new OtherCenter();
            oc_ptr->start_time = 0;
            oc_ptr->center_status = LOOKING;
            oc_ptr->vote_count = 1;
        } else {
            OCPTR& oc_ptr = iter->second;
            vote_count = ++oc_ptr->vote_count;
        }
    }

    if (0 != election_done_num_ && vote_count >= election_done_num_) {
        /*
         * 当前投票结果已经有票数超过 n / 2 + 1, 结果产出
         */
        UpdateLeadingCenter(centers_proto.leader_center());
    }

    return true;
}

void RpcCenter::UpdateLeadingCenter(const string& leader_center) {
    WriteLockGuard wguard(lc_rwlock_);
    leader_center_ = leader_center;
}

void RpcCenter::IncreaseLogicalClock() {
    WriteLockGuard wguard(logical_clock_rwlock_);
    ++logical_clock_;
}

CenterStatus RpcCenter::GetLocalCenterStatus() {
    WriteLockGuard wguard(status_rwlock_);
    return center_status_;
}

time_t RpcCenter::GetOCStartTime(const string& center) {
    WriteLockGuard wguard(oc_rwlock_);
    HashMap::iterator iter = other_centers_ptr_->find(center);
    if (other_centers_ptr_->end() == iter) {
        return 0;
    }
    return iter->second->start_time;
}

string RpcCenter::GetLeadingCenter() {
    WriteLockGuard wguard(lc_rwlock_);
    return leader_center_;
}

unsigned long RpcCenter::GetLogicalClock() {
    WriteLockGuard wguard(logical_clock_rwlock_);
    return logical_clock_;
}

CenterStatus RpcCenter::GetCenterStatus() {
    ReadLockGuard wguard(status_rwlock_);
    return center_status;
}


CenterAction RpcCenter::FastLeaderElection(const CentersProto& centers_proto) {
    CenterAction ca = LeaderPredicate(centers_proto);

    if (ACCEPT == ca) {
        UpdateOCStatus(centers_proto);
    }
    return ca;
}

bool RpcCenter::ProposalLeaderElection(const char* recommend_center) {
    /*
     * 遍历所有已知Center服务器, 获取每个Center服务状态，以及其Follow的Leader机器
     * 同时尝试进行election选举
     */
    CountMap count_map;
    CentersProto proposal;
    proposal.set_center_status(LOOKING);
    proposal.set_center_action(PROPOSAL);
    proposal.set_start_time(start_time_);
    proposal.set_lc_start_time(start_time_);
    proposal.set_logical_clock(logical_clock_);
    proposal.set_leader_center(recommend_center);

    string proposal_str;
    if (proposal.SerializeToString(&proposal)) {
        return false;
    }
    {
        WriteLockGuard wguard(oc_rwlock_);
        for (HashMap::iterator iter = other_centers_ptr_->begin();
             iter != other_centers_ptr_->end();
             ++iter) {
            string& center_addr = iter->first;
            int32_t conn_fd = TcpConnect(center_addr.c_str(), center_port_, 15);
            if (conn_fd <= 0) {
                continue;
            }
            /*
             * 群发每个Center发送Proposal
             */
            if (RpcSend(conn_fd, 0, proposal_str, false)) {
                close(conn_fd);
            }
            close(conn_fd);
        }
    }

    /*
     * Proposal结束, 更新logical_clock
     */
    IncreaseLogicalClock();
    return true;
}

CenterAction RpcCenter::LeaderPredicate(const CentersProto& centers_proto) {
    unsigned long logical_clock = GetLogicalClock();
    if (centers_proto.logical_clock() < logical_clock) {
        return REFUSED;
    } else if (centers_proto.logical_clock() == logical_clock) {
        string lc = GetLeadingCenter();
        time_t lc_start_time = GetOCStartTime(lc);
        if (centers_proto.lc_start_time() > lc_start_time) {
            return REFUSED;
        } else if (centers_proto.lc_start_time() == lc_start_time &&
                   lc.compare(centers_proto.leader_center()) >= 0) {
            /*
             * 启动时间相等时, 如果新Leader大于目前的Leader, 不更新目前leader
             */
            return REFUSED;
        }
    }

    return ACCEPT;
}

bool RpcCenter::CenterProcessor(int32_t conn_fd) {
    string recv_message;
    int32_t center_type = RpcRecv(conn_fd, recv_message, true);
    if (center_type < 0) {
        return;
    }
    switch (center_type) {
       case CENTER2CENTER : {
            /*
             * 处理来自其他Center服务器的请求
             */
            if (NULL == election_thread_) {
                return false;
            }
            election_thread_->PushElectionMessage(recv_message);
            break;
       }
       case CENTER2CLIENT : {
            /*
             * 处理来自RPC客户端的请求
             */
            CenterClient center_client;
            center_client.ParseFromString(recv_message);
            break;
       }
       case CENTER2CLUSTER : {
            /*
             * 处理来自RPC服务集群的请求
             */
            CenterCluster center_cluster;
            center_cluster.ParseFromString(recv_message);
            break;
       }
       default:
            return false;
    }
    return true;
}

}  // end of namespace libevrpc











/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
