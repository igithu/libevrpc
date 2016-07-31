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
    start_time_(time(0)),
    leader_infos_ptr_(new LeaderInfos());
    other_centers_ptr_(new HashMap()),
    // leader_center_(GetLocalAddress()),
    election_done_num_(0),
    logical_clock_(0),
    fastleader_election_running_(false),
    center_server_thread_(NULL),
    election_thread_(NULL),
    reporter_thread_(NULL) {

    leader_infos_ptr_->lc_start_time = start_time_;
    leader_infos_ptr_->leader_center = GetLocalAddress();
}

RpcCenter::~RpcCenter() {
    if (NULL != leader_infos_ptr_) {
        delete leader_infos_ptr_;
    }

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

    if (!center_server_thread_->Start()) {
        fprintf(stderr, "Start the center thread failed!\n");
        return false;
    }

    if (!StartFastLeaderElection()) {
        fprintf(stderr, "Start the election thread failed!\n");
        return false;
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
        LeaderInfos li = {centers_proto.lc_start_time, centers_proto.leader_center()}
        UpdateLeadingCenterInfos(li);
        /*
         * 投票结果产出 终止选票线程
         */
        if (strcmp(leader_center.c_str(), GetLocalAddress()) == 0) {
            /**
             * 确认当前Center服务器为 Leader服务器
             */
            UpdateCenterStatus(LEADING);
            CentersProto confirm_leader;
            /*
             * TODO 开始向所有机器广播 确认Leader身份
             */

        } else {
            UpdateCenterStatus(OBSERVING);
        }
        election_thread_->Stop();
    }

    return true;
}

void RpcCenter::UpdateLeadingCenterInfos(const LeaderInfos& leader_infos) {
    WriteLockGuard wguard(lc_rwlock_);
    leader_infos_ptr_->leader_center = leader_infos.leader_center;
    leader_infos_ptr_->lc_start_time = leader_infos.lc_start_time;
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
    return leader_infos_ptr_->leader_center;
}

unsigned long RpcCenter::GetLogicalClock() {
    WriteLockGuard wguard(logical_clock_rwlock_);
    return logical_clock_;
}

CenterStatus RpcCenter::GetCenterStatus() {
    ReadLockGuard rguard(status_rwlock_);
    return center_status;
}


bool RpcCenter::FastLeaderElection(const CentersProto& centers_proto) {
    CenterAction ca = LeaderPredicate(centers_proto);

    if (ACCEPT == ca) {
        UpdateOCStatus(centers_proto);
    }

    int32_t conn_fd = TcpConnect(centers_proto.from_center_addr().c_str(), center_port_, 15);
    if (conn_fd <= 0) {
        return false;
    }

    string lc_center = GetLeadingCenter();

    CentersProto response_proto;
    response_proto.set_from_center_addr(GetLocalAddress());
    response_proto.set_center_status(GetCenterStatus());
    response_proto.set_center_action(ca);
    response_proto.set_start_time(start_time_);
    response_proto.set_lc_start_time(GetOCStartTime(lc_center));
    response_proto.set_logical_clock(GetLogicalClock());
    response_proto.set_leader_center(lc_center);

    string response_str;
    response_proto.SerializeToString(response_str);

    if (!RpcSend(conn_fd, 0, proposal_str, false)) {
        fprintf(stderr, "FastLeaderElection send to %s failed!\n", centers_proto.from_center_addr().c_str());
    }
    close(conn_fd);

    return true;
}

bool RpcCenter::ProposalLeaderElection(const char* recommend_center) {
    /*
     * 遍历所有已知Center服务器, 获取每个Center服务状态，以及其Follow的Leader机器
     * 同时尝试进行election选举
     */
    CountMap count_map;
    CentersProto proposal;
    proposal.set_from_center_addr(GetLocalAddress());
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
    BroadcastInfo(proposal_str);

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

bool RpcCenter::StartFastLeaderElection() {
    if (!IsFastLeaderRunning()) {
        if (!election_thread_->Start()) {
            return false;
        }
        SetFastLeaderRunning(true);
    }
    return true;
}

bool RpcCenter::BroadcastInfo(const std::string& bc_info) {
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
        if (!RpcSend(conn_fd, 0, bc_info, false)) {
            fprintf(stderr, "Proposal send to %s failed!\n", center_addr.c_str());
        }
        close(conn_fd);
    }

    return true;
}


void RpcCenter::SetFastLeaderRunning(bool is_running) {
    WriteLockGuard wguard(fle_running_rwlock_);
    fastleader_election_running_ = is_running;
}

bool RpcCenter::IsFastLeaderRunning() {
    ReadLockGuard rguard(fle_running_rwlock_);
    return fastleader_election_running_;
}

}  // end of namespace libevrpc











/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
