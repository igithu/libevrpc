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
    start_time_(time(0)),
    logical_clock_(0) {
}

RpcCenter::~RpcCenter() {
    if (NULL != other_centers_ptr_) {
        delete other_centers_ptr_;
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

    std::ifstream in(cfile);
    string line;
    while (getline (in, line)) {
        /*
         * 初始启动
         */
        OCPTR oc_ptr = new OtherCenter();
        memet(oc_ptr, 0, sizeof(OtherCenter));
        other_centers_ptr_->put(line, oc_ptr);
    }

    return true;
}

bool RpcCenter::StartCenter() {
    if (!InitRpcCenter()) {
        return false;
    }

    if (!FastLeaderElection()) {
        fprintf(stderr, "Run the Ecletion failed!\n");
    }

    return true;
}

void RpcCenter::UpdateCenterStatus(CenterStatus cs) {
    WriteLockGuard wguard(status_rwlock_);
    center_status_ = cs;
}

void RpcCenter::UpdateOCStatus(const string& addr, OCPTR& oc_ptr) {
    if (NULL == other_centers_ptr_) {
        return;
    }
    WriteLockGuard wguard(oc_rwlock_);
    HashMap::iterator iter = other_centers_ptr_->find(addr);
    if (iter == other_centers_ptr_->end()) {
        other_centers_ptr_->insert(std::make_pair(addr, oc_ptr));
    } else {
        iter->second = oc_ptr;
        // strcpy(oc_ptr->lead_center, rec_addr.c_str());
        // oc_ptr->center_status = cs;
    }
}

void RpcCenter::UpdateLeadingCenter(const string& lead_center) {
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
    return lead_center;
}

unsigned long RpcCenter::GetLogicalClock() {
    WriteLockGuard wguard(logical_clock_rwlock_);
    return logical_clock_;
}


bool RpcCenter::FastLeaderElection(const char* recommend_center) {
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

            strong response_str;
            if (!RpcRecv(conn_fd, response_str, false)) {
                close(conn_fd);
            }
            CentersProto respone_proto;
            if (!respone_proto.ParseFromString(response_str)) {
                continue;
            }
            string candidate;
            switch (respone_proto.center_action()) {
                case ACCEPT:
                    candidate = recommend_center;
                    break;
                case REFUSED:
                    candidate = respone_proto.lead_center();
                    break;
            }
            ++count_map[candidate];

            OCPTR oc_ptr = new OtherCenter();
            oc_ptr->start_time = respone_proto.start_time();
            oc_ptr->center_status = respone_proto.center_status();
            strcpy(oc_ptr->lead_center, candidate.c_str());
            UpdateOCStatus(center_addr, oc_ptr);
            close(conn_fd);
        }
    }

    /*
     * 本轮投票结束, 更新logical_clock
     */
    IncreaseLogicalClock();
    return true;
}

CenterAction RpcCenter::LeaderPredicate(const CentersProto& center_proto) {
    unsigned long logical_clock = GetLogicalClock();
    if (center_proto.logical_clock() < logical_clock) {
        return REFUSED;
    } else if (center_proto.logical_clock() == logical_clock) {
        string lc = GetLeadingCenter();
        time_t lc_start_time = GetOCStartTime(lc);
        if (center_proto.lc_start_time() > lc_start_time) {
            return REFUSED;
        } else if (center_proto.lc_start_time() == lc_start_time &&
                   lc.compare(center_proto.leader_center()) >= 0) {
            /*
             * 启动时间相等时, 如果新Leader大于目前的Leader, 不更新目前leader
             */
            return REFUSED;
        }

    UpdateLeadingCenter(center_proto.lead_center());
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
            CentersProto centers_proto;
            centers_proto.ParseFromString(recv_message);
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
