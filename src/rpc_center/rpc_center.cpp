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

#include <unistd.h>
#include <time.h>
#include <fstream>
#include <iostream>

#include <vector>

#include <google/protobuf/repeated_field.h>

#include "center_proto/center_type.pb.h"
#include "center_proto/center_client.pb.h"
#include "util/rpc_communication.h"
#include "util/rpc_util.h"
#include "rpc_center/load_balancer/consistent_hash_load_balancer.h"

namespace libevrpc {

using std::string;
using std::vector;
using ::google::protobuf::RepeatedPtrField;

extern string g_config_file = "";

RpcCenter::RpcCenter(const string& config_file) :
    center_port_(NULL),
    config_parser_instance_(ConfigParser::GetInstance(config_file)),
    center_status_(LOOKING),
    start_time_(time(0)),
    leader_infos_ptr_(new LeaderInfos()),
    other_centers_ptr_(new HashMap()),
    election_done_num_(0),
    logical_clock_(0),
    fastleader_election_running_(false),
    center_server_thread_(NULL),
    election_thread_(NULL),
    reporter_thread_(NULL),
    leader_thread_(NULL),
    load_balancer_ptr_(NULL),
    center_hash_map_ptr_(new CENTER_HASH_MAP()),
    rpc_server_buf_ptr_(new SERVER_SET()) {

    leader_infos_ptr_->lc_start_time = start_time_;
    leader_infos_ptr_->leader_center = GetLocalAddress();
    g_config_file = config_file;

}

RpcCenter::~RpcCenter() {
    if (NULL != leader_infos_ptr_) {
        delete leader_infos_ptr_;
    }

    if (NULL != other_centers_ptr_) {
        delete other_centers_ptr_;
    }

    if (NULL != center_server_thread_) {
        delete center_server_thread_;
    }

    if (NULL != election_thread_) {
        delete election_thread_;
    }

    if (NULL != reporter_thread_) {
        delete reporter_thread_;
    }

    if (NULL != center_port_) {
        free(center_port_);
    }

    if (NULL != leader_thread_) {
        delete leader_thread_;
    }

    if (NULL != load_balancer_ptr_) {
        delete load_balancer_ptr_;
    }

    if (NULL != center_hash_map_ptr_) {
        delete center_hash_map_ptr_;
    }

    if (NULL != rpc_server_buf_ptr_) {
        delete rpc_server_buf_ptr_;
    }
}

RpcCenter& RpcCenter::GetInstance(const std::string& config_file) {
    static RpcCenter rc_instance(config_file);
    return rc_instance;
}

bool RpcCenter::InitRpcCenter() {
    const string cfile = "/tmp/centers.data";
    int ftyp = access(cfile.c_str(), 0);
    if (0 != ftyp) {
        /*
         * 需要的服务器列表文件不存在，无法初始化后与其他机器进行通信
         * 启动失败!!!
         */
        return false;
    }

    const char* local_addr = GetLocalAddress();
    const char* local_port = config_parser_instance_.IniGetString("rpc_center:port", "8899");

    center_port_ = (char*)malloc(strlen(local_port));
    strcpy(center_port_, local_port);


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
        OCPTR oc_ptr(new OtherCenter());
        oc_ptr->vote_count = 0;
        oc_ptr->center_status = UNKONW;
        memset(oc_ptr.get(), 0, sizeof(OtherCenter));
        other_centers_ptr_->insert(std::make_pair(line, oc_ptr));
        RegistNewCenter(line);
    }

    election_done_num_ = (other_centers_ptr_->size() + 1) / 2 + 1;

    /*
     * 初始化线程指针
     */
    center_server_thread_ = new CenterServerThread(GetLocalAddress(), local_port);
    election_thread_ = new ElectionThread();
    reporter_thread_ = new ReporterThread();
    leader_thread_ = new LeaderThread();

    /*
     * 初始化负载均衡
     */
    int32_t load_balancer_policy = config_parser_instance_.IniGetInt("rpc_center:load_balancer_policy", 1);
    switch (load_balancer_policy) {
        case 1 : load_balancer_ptr_ = new ConsistentHashLoadBalancer(g_config_file); break;
        default: load_balancer_ptr_ = new ConsistentHashLoadBalancer(g_config_file);
    }
    load_balancer_ptr_->SetConfigFile(g_config_file);
    load_balancer_ptr_->InitBalancer();

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

bool RpcCenter::StopCenter() {
    if (NULL != election_thread_) {
        election_thread_->Stop();
    }

    if (NULL != reporter_thread_) {
        reporter_thread_->Stop();
    }

    if (NULL != center_server_thread_) {
        center_server_thread_->Stop();
    }

    return true;
}

void RpcCenter::WaitCenter() {
    if (NULL != center_server_thread_) {
        center_server_thread_->Wait();
    }

    if (NULL != election_thread_) {
        election_thread_->Wait();
    }

    if (NULL != reporter_thread_) {
        reporter_thread_->Wait();
    }

    if (NULL != leader_thread_) {
        leader_thread_->Wait();
    }
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
            OCPTR oc_ptr(new OtherCenter());
            oc_ptr->start_time = centers_proto.start_time();
            oc_ptr->center_status = centers_proto.center_status();
            oc_ptr->vote_count = 0;
            strcpy(oc_ptr->current_follow_leader_center, centers_proto.leader_center().c_str());
            other_centers_ptr_->insert(std::make_pair(addr, oc_ptr));
        } else {
            OCPTR& oc_ptr = iter->second;
            oc_ptr->start_time = centers_proto.start_time();
            oc_ptr->center_status = centers_proto.center_status();
            strcpy(oc_ptr->current_follow_leader_center, centers_proto.leader_center().c_str());
        }
    }

    uint32_t vote_count = 0;
    {
        WriteLockGuard wguard(oc_rwlock_);
        /*
         * 更新当前中心服务器投票的信息
         */
        HashMap::iterator leader_iter = other_centers_ptr_->find(centers_proto.leader_center());
        if (leader_iter == other_centers_ptr_->end()) {
            OCPTR oc_ptr(new OtherCenter());
            oc_ptr->start_time = 0;
            oc_ptr->center_status = LOOKING;
            oc_ptr->vote_count = 1;
            other_centers_ptr_->insert(std::make_pair(addr, oc_ptr));
        } else {
            OCPTR& oc_ptr = leader_iter->second;
            vote_count = ++oc_ptr->vote_count;
        }
    }

    if (0 != election_done_num_ && vote_count >= election_done_num_) {
        /*
         * 当前投票结果已经有票数超过 n / 2 + 1, 结果产出
         */
        UpdateLeadingCenterInfos(centers_proto);
        /*
         * 投票结果产出 终止选票线程
         */
        string leader_center = GetLeadingCenter();
        if (strcmp(leader_center.c_str(), GetLocalAddress()) == 0) {
            /**
             * 确认当前Center服务器为 Leader服务器
             */
            UpdateCenterStatus(LEADING);
            if (!leader_thread_->Start()) {
                return false;
            }

            CentersProto confirm_proto;
            confirm_proto.set_from_center_addr(GetLocalAddress());
            confirm_proto.set_center_action(LEADER_CONFIRM);
            confirm_proto.set_start_time(start_time_);
            confirm_proto.set_logical_clock(GetLogicalClock());
            confirm_proto.set_center_status(LEADING);
            confirm_proto.set_lc_start_time(start_time_);
            confirm_proto.set_leader_center(GetLocalAddress());

            string confirm_str;
            if (confirm_proto.SerializeToString(&confirm_str)) {
                BroadcastInfo(confirm_str);
            }
            election_thread_->Stop();
        } else {
            /**
             * 确认当前Center服务器进入OBSERVING状态
             */
            if (FOLLOWING != GetCenterStatus()) {
                UpdateCenterStatus(OBSERVING);
            }
        }
    }

    return true;
}

void RpcCenter::UpdateLeadingCenterInfos(const CentersProto& centers_proto) {
    WriteLockGuard wguard(lc_rwlock_);
    leader_infos_ptr_->lc_start_time = centers_proto.lc_start_time();
    leader_infos_ptr_->leader_center = centers_proto.leader_center();
}

void RpcCenter::IncreaseLogicalClock() {
    WriteLockGuard wguard(logical_clock_rwlock_);
    ++logical_clock_;
}

CenterStatus RpcCenter::GetLocalCenterStatus() {
    ReadLockGuard rguard(status_rwlock_);
    return center_status_;
}

time_t RpcCenter::GetOCStartTime(const string& center) {
    ReadLockGuard rguard(oc_rwlock_);
    HashMap::iterator iter = other_centers_ptr_->find(center);
    if (other_centers_ptr_->end() == iter) {
        return 0;
    }
    return iter->second->start_time;
}

string RpcCenter::GetLeadingCenter() {
    ReadLockGuard rguard(lc_rwlock_);
    return leader_infos_ptr_->leader_center;
}

time_t RpcCenter::GetLeadingCenterStartTime() {
    ReadLockGuard rguard(lc_rwlock_);
    return leader_infos_ptr_->lc_start_time;
}

unsigned long RpcCenter::GetLogicalClock() {
    ReadLockGuard rguard(logical_clock_rwlock_);
    return logical_clock_;
}

CenterStatus RpcCenter::GetCenterStatus() {
    ReadLockGuard rguard(status_rwlock_);
    return center_status_;
}

bool RpcCenter::FastLeaderElection() {
    InquiryCenters();
    ProposalLeaderElection();
    return true;
}

bool RpcCenter::InquiryCenters() {
    CentersProto inquiry_proto;
    inquiry_proto.set_from_center_addr(GetLocalAddress());
    inquiry_proto.set_center_status(GetCenterStatus());
    inquiry_proto.set_center_action(INQUIRY);
    inquiry_proto.set_start_time(start_time_);
    inquiry_proto.set_logical_clock(GetLogicalClock());

    string inquiry_str;
    if (!inquiry_proto.SerializeToString(&inquiry_str)) {
        return false;
    }

    WriteLockGuard wguard(oc_rwlock_);
    for (HashMap::iterator iter = other_centers_ptr_->begin();
         iter != other_centers_ptr_->end();
         ++iter) {
        OCPTR oc_ptr = iter->second;
        if (UNKONW != oc_ptr->center_status) {
            /*
             * 询问阶段 如果不是UNKONW，说明该服务机器已经同步最新机器到本地，忽略
             */
            continue;
        }
        const string& center_addr = iter->first;
        int32_t conn_fd = TcpConnect(center_addr.c_str(), center_port_, 15);
        if (conn_fd <= 0) {
            continue;
        }
        /*
         * 群发每个Center发送Proposal
         */
        if (!RpcSend(conn_fd, CENTER2CENTER, inquiry_str)) {
            fprintf(stderr, "Proposal send to %s failed!\n", center_addr.c_str());
        }

        string recv_message;
        int32_t center_type = RpcRecv(conn_fd, recv_message, false);
        if (center_type < 0) {
            continue;
        }
        CentersProto response_proto;
        if (response_proto.ParseFromString(recv_message)) {
            oc_ptr->start_time = response_proto.start_time();
            oc_ptr->center_status = response_proto.center_status();
            /*
             * 询问阶段主要以被询问的机器为目标，判断其是否具有Leader资格
             */
            response_proto.set_lc_start_time(response_proto.start_time());
            response_proto.set_leader_center(response_proto.from_center_addr());
            if (ACCEPT == LeaderPredicate(response_proto)) {
                UpdateLeadingCenterInfos(response_proto);
            }
        }
        close(conn_fd);
    }

    return true;
}

bool RpcCenter::ProposalLeaderElection() {
    /*
     * Proposal开始前, 更新logical_clock
     */
    IncreaseLogicalClock();

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
    proposal.set_lc_start_time(GetLeadingCenterStartTime());
    proposal.set_logical_clock(GetLogicalClock());
    proposal.set_leader_center(GetLeadingCenter());

    string proposal_str;
    if (proposal.SerializeToString(&proposal_str)) {
        return false;
    }
    BroadcastInfo(proposal_str);

    return true;
}

CenterAction RpcCenter::LeaderPredicate(const CentersProto& centers_proto) {
    unsigned long logical_clock = GetLogicalClock();
    if (centers_proto.logical_clock() < logical_clock) {
        return REFUSED;
    } else if (centers_proto.logical_clock() == logical_clock) {
        string lc = GetLeadingCenter();
        time_t lc_start_time = GetLeadingCenterStartTime();
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
    int32_t center_type = RpcRecv(conn_fd, recv_message, false);
    if (center_type < 0) {
        close(conn_fd);
        return false;
    }

    switch (center_type) {
       case CENTER2CENTER : {
            /*
             * 处理来自其他Center服务器的请求
             */
            CentersProto response_proto;
            if (response_proto.ParseFromString(recv_message)) {
                return false;
            }
            switch (response_proto.center_action()) {
                case INQUIRY: case PROPOSAL: case ACCEPT: case REFUSED: case LEADER_CONFIRM: {
                    if (NULL == election_thread_) {
                        return false;
                    }
                    election_thread_->PushElectionMessage(conn_fd, response_proto);
                    break;
                }
                default: {
                    CentersProto response_proto;
                    if (GetCenterStatus() != LEADING) {
                        /*
                         * 本地Center非Leader直接拒绝请求
                         */
                        response_proto.set_from_center_addr(GetLocalAddress());
                        response_proto.set_center_action(REFUSED);
                        string response_str;
                        if (!response_proto.SerializeToString(&response_str)) {
                            return false;
                        }
                        if (!RpcSend(conn_fd, CENTER2CENTER, response_str)) {
                            return false;
                        }
                    } else {
                        leader_thread_->PushFollowerMessage(conn_fd, response_proto);
                    }
                    break;
                }
            }
            break;
       }
       case CENTER2CLIENT : {
            /*
             * 处理来自RPC客户端的请求
             */
            ClientWithCenter cwc_proto;
            if (cwc_proto.ParseFromString(recv_message)) {
                return false;
            }
            string client_addr = cwc_proto.from_addr();
            ClientWithCenter cwc_response_proto;
            switch (cwc_proto.client_center_action()) {
                case CLIENT_INIT_REQ: {
                    uint32_t hash_id = MurMurHash2(client_addr.c_str(), client_addr.size());
                    {
                        ReadLockGuard rguard(center_hash_map_rwlock_);
                        CENTER_HASH_MAP::iterator iter = center_hash_map_ptr_->lower_bound(hash_id);
                        for (int32_t i = 0; i < 3; ++i) {
                            if (center_hash_map_ptr_->end() != iter) {
                                ++iter;
                            } else {
                                iter = center_hash_map_ptr_->begin();
                            }
                            cwc_response_proto.add_should_communicate_center(iter->second);
                        }
                    }

                    vector<string> server_list;
                    if (load_balancer_ptr_->GetRpcServer(client_addr, server_list)) {
                        for (vector<string>::iterator iter = server_list.begin();
                             iter != server_list.end();
                             ++iter) {
                            cwc_response_proto.add_cluster_server_list(*iter);
                        }
                    }

                    break;
                }
                case UPDATE_SERVER_INFO: {
                    vector<string> server_list;
                    if (load_balancer_ptr_->GetRpcServer(client_addr, server_list)) {
                        for (vector<string>::iterator iter = server_list.begin();
                             iter != server_list.end();
                             ++iter) {
                            cwc_response_proto.add_cluster_server_list(*iter);
                        }
                    }

                    break;
                }
                default:
                    break;
            }

            string response_client_str;
            if (!cwc_response_proto.SerializeToString(&response_client_str) ||
                !RpcSend(conn_fd, CENTER2CLIENT, response_client_str)) {
                return false;
            }

            break;
       }
       case CENTER2CLUSTER : {
            if (!CenterIsReady()) {
                CenterResponseCluster crc;
                crc.set_center_response_action(CENTER_NOT_READY);

                string response_str;
                crc.SerializeToString(&response_str);
                if (!RpcSend(conn_fd, CENTER2CLUSTER, response_str)) {
                    return false;
                }
                return false;
            }

            /*
             * 处理来自RPC服务集群的请求
             */
            RpcClusterServer rpc_cluster_server;
            if (!rpc_cluster_server.ParseFromString(recv_message)) {
                return false;
            }
            switch (rpc_cluster_server.cluster_action()) {
                case REGISTER: {
                    /*
                     * 1.如果接收的Center是Leader机器, 则将当前RpcServer机器信息直接写进到LoadBalancer(但是通常架构中
                     *   RpcServer 应该不会向Leader机器直接请求信息)
                     * 2.如果接收的Center是Follower机器, 则将当前RpcServer机器先写入本地的Buf中,然后由本地心跳线程将Buff中的数据
                     *   汇报Leader机器中 进行LoadBalancer计算
                     */
                    CenterStatus cur_cs = GetCenterStatus();
                    if (LEADING == cur_cs) {
                        load_balancer_ptr_->AddRpcServer(rpc_cluster_server);
                    } else {
                        AddRpcServerToBuf(rpc_cluster_server);
                    }

                    string server_addr = rpc_cluster_server.cluster_server_addr();
                    CenterResponseCluster crc;
                    crc.set_center_response_action(CLUSTER_RESP);
                    uint32_t hash_id = MurMurHash2(server_addr.c_str(), server_addr.size());
                    {
                        ReadLockGuard rguard(center_hash_map_rwlock_);
                        CENTER_HASH_MAP::iterator iter = center_hash_map_ptr_->lower_bound(hash_id);
                        for (int32_t i = 0; i < 3; ++i) {
                            if (center_hash_map_ptr_->end() != iter) {
                                ++iter;
                            } else {
                                iter = center_hash_map_ptr_->begin();
                            }
                            crc.add_should_reporter_center(iter->second);
                        }
                    }
                    string send_str;
                    if (crc.SerializeToString(&send_str)) {
                        if (!RpcSend(conn_fd, CENTER2CLUSTER, send_str)) {
                            return false;
                        }
                    }
                    break;
                }
                case CLUSTER_PING: {
                    break;
                }
            }
            break;
       }
       default:
            return false;
    }
    return true;
}

bool RpcCenter::ProcessCenterData(int32_t fd, const CentersProto& centers_proto) {
    CenterAction ca = centers_proto.center_action();

    switch (ca) {
        case INQUIRY: {
            /**
             * 仅仅返回本地相应的Center信息
             */
            string lc_center = GetLeadingCenter();
            CentersProto response_proto;
            response_proto.set_from_center_addr(GetLocalAddress());
            response_proto.set_center_status(GetCenterStatus());
            response_proto.set_center_action(ca);
            response_proto.set_start_time(start_time_);
            response_proto.set_lc_start_time(GetLeadingCenterStartTime());
            response_proto.set_logical_clock(GetLogicalClock());
            response_proto.set_leader_center(lc_center);

            string response_str;
            if (!response_proto.SerializeToString(&response_str)) {
                close(fd);
                return false;
            }

            if (!RpcSend(fd, CENTER2CENTER, response_str)) {
                fprintf(stderr, "FastLeaderElection send to %s failed!\n", centers_proto.from_center_addr().c_str());
            }

            break;
        }
        case PROPOSAL: {
            CenterAction ca_result = LeaderPredicate(centers_proto);
            if (ACCEPT == ca_result) {
                UpdateOCStatus(centers_proto);
            }
            break;
        }
        case LEADER_CONFIRM: {
            CenterAction ca_result = LeaderPredicate(centers_proto);
            if (NULL != reporter_thread_) {
                reporter_thread_->Start();
            } else {
                fprintf(stderr, "Start reporter_thread error! reporter_thread ptr is NULL!\n");
            }
            if (ACCEPT == ca_result) {
                UpdateCenterStatus(FOLLOWING);
                if (leader_thread_->IsAlive()) {
                    leader_thread_->Stop();
                }
            }
            break;
        }
        case FOLLOWER_PING: {
            const RepeatedPtrField<RpcClusterServer>& server_infos_list = centers_proto.server_infos_list();
            for (RepeatedPtrField<RpcClusterServer>::const_iterator iter = server_infos_list.begin();
                 iter != server_infos_list.end();
                 ++iter) {
                load_balancer_ptr_->AddRpcServer(*iter);
            }

            CentersProto cp_response;
            cp_response.set_center_action(LEADER_PING_RESPONSE);
            RepeatedPtrField<LoadBalancerMetaData>* lb_result_list = cp_response.mutable_lb_result();
            load_balancer_ptr_->GetCurrentLBResult(*lb_result_list);

            string cp_response_str;
            if (!cp_response.SerializeToString(&cp_response_str)) {
                close(fd);
                return false;
            }
            if (!RpcSend(fd, CENTER2CENTER, cp_response_str)) {
                fprintf(stderr, "FastLeaderElection send to %s failed!\n", centers_proto.from_center_addr().c_str());
            }

            break;
        }
        case LEADER_PING_RESPONSE: {
            // const RepeatedPtrField<LoadBalancerMetaData>* lb_result_list_ptr = centers_proto.mutable_lb_result();
            const RepeatedPtrField<LoadBalancerMetaData>& lb_result_list = centers_proto.lb_result();
            load_balancer_ptr_->UpdateLBResult(lb_result_list);
            break;
        }
        default:
            break;
    }
    close(fd);
    return true;
}

bool RpcCenter::StartFastLeaderElection() {
    if (!IsFastLeaderRunning()) {
        if (!election_thread_->Start()) {
            return false;
        }
        SetFastLeaderRunning(true);
    }
    FastLeaderElection();
    return true;
}

bool RpcCenter::BroadcastInfo(std::string& bc_info) {
    ReadLockGuard rguard(oc_rwlock_);
    for (HashMap::iterator iter = other_centers_ptr_->begin();
         iter != other_centers_ptr_->end();
         ++iter) {
        const string& center_addr = iter->first;
        int32_t conn_fd = TcpConnect(center_addr.c_str(), center_port_, 15);
        if (conn_fd <= 0) {
            continue;
        }
        /*
         * 群发每个Center发送Proposal
         */
        if (!RpcSend(conn_fd, CENTER2CENTER, bc_info)) {
            fprintf(stderr, "Proposal send to %s failed!\n", center_addr.c_str());
        }
        close(conn_fd);
    }

    return true;
}

bool RpcCenter::ReporterProcessor(int32_t conn_fd) {
    sleep(1);
    CentersProto centers_proto;
    centers_proto.set_from_center_addr(GetLocalAddress());
    centers_proto.set_center_action(FOLLOWER_PING);

    /*
     * 取出Buf中的数据, 然后将这些收集到的汇报给Leader机器, 由Leader机器统一计算做负载均衡计算等操作
     */
    if (NULL != rpc_server_buf_ptr_ && !rpc_server_buf_ptr_->empty()) {
        ReadLockGuard rguard(rpc_server_buf_rwlock_);
        for (SERVER_SET::iterator iter = rpc_server_buf_ptr_->begin();
             iter != rpc_server_buf_ptr_->end();
             ++iter) {
            RpcClusterServer* rcs = centers_proto.add_server_infos_list();
            rcs->CopyFrom(*iter);
        }
    }

    string ping_str;
    if (!centers_proto.SerializeToString(&ping_str)) {
        return false;
    }
    if (!RpcSend(conn_fd, CENTER2CENTER, ping_str)) {
        return false;
    }
    return CenterProcessor(conn_fd);
    // return true;
}

void RpcCenter::SetFastLeaderRunning(bool is_running) {
    WriteLockGuard wguard(fle_running_rwlock_);
    fastleader_election_running_ = is_running;
}

bool RpcCenter::IsFastLeaderRunning() {
    ReadLockGuard rguard(fle_running_rwlock_);
    return fastleader_election_running_;
}

bool RpcCenter::RegistNewCenter(const string& new_center) {
    WriteLockGuard wguard(center_hash_map_rwlock_);
    for (int32_t i = 0; i < 50; ++i) {
        string hash_str = "SHARD-" + new_center + "-NODE-" + static_cast<char>(i);
        center_hash_map_ptr_->insert(std::make_pair(MurMurHash2(hash_str.c_str(), hash_str.size()), new_center));
    }
    return true;
}

bool RpcCenter::CenterIsReady() {
    CenterStatus cs = GetCenterStatus();
    if (FOLLOWING != cs && LEADING != cs) {
        return false;
    }
    return true;
}

bool RpcCenter::AddRpcServerToBuf(const RpcClusterServer& rpc_cluster_server) {
    WriteLockGuard wguard(rpc_server_buf_rwlock_);
    rpc_server_buf_ptr_->insert(rpc_cluster_server);
    return true;
}

}  // end of namespace libevrpc











/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
