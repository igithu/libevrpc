/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file consistent_hash_load_balancer.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2016/09/08 20:22:19
 * @brief
 *
 **/


#include "consistent_hash_load_balancer.h"

#include "config_parser/config_parser.h"
#include "util/rpc_util.h"


namespace libevrpc {

using std::string;
using std::vector;

ConsistentHashLoadBalancer::ConsistentHashLoadBalancer() :
    config_file_(""),
    virtual_node_num_(20),
    vn_map_ptr_(new VN_HASH_MAP()),
    py_server_list_ptr_(new vector<PN_PTR>()) {
}

ConsistentHashLoadBalancer::~ConsistentHashLoadBalancer() {
    if (NULL != vn_map_ptr_) {
        delete vn_map_ptr_;
    }
    if (NULL != py_server_list_ptr_) {
        delete py_server_list_ptr_;
    }
}

bool ConsistentHashLoadBalancer::InitBalancer() {
    if ("" == config_file_) {
        return false;
    }
    ConfigParser& config_parser_instance = ConfigParser::GetInstance(config_file_);
    virtual_node_num_ = config_parser_instance.IniGetInt("rpc_center:virtual_node_num", 20);
    return true;
}

void ConsistentHashLoadBalancer::SetConfigFile(const std::string& file_name) {
    config_file_ = file_name;
}

bool ConsistentHashLoadBalancer::AddRpcServer(const RpcClusterServer& rpc_server) {
    PN_PTR rcs_ptr(new RpcClusterServer());
    rcs_ptr->CopyFrom(rpc_server);
    py_server_list_ptr_->push_back(rcs_ptr);

    VN_PTR vn_ptr = new VirtualNode();
    for (int32_t i = 0; i < rcs_ptr->should_reporter_center_size(); ++i) {
        vn_ptr->py_node_list.push_back(rcs_ptr->should_reporter_center(i))
    }

    WriteLockGuard wguard(vmap_rwlock_);
    for (int32_t i = 0; i < virtual_node_num_; ++i) {
        string hash_str = "SHARD-" + rcs_ptr->cluster_server_addr() + "-NODE-" + static_cast<char>(i);
        vn_map_ptr_->insert(std::make_pair(MurMurHash2(hash_str.c_str(), hash_str.size()), vn_ptr));
    }
    return true;
}

bool ConsistentHashLoadBalancer::GetRpcServer(
        const string& rpc_client, vector<string>& rpc_server_list) {
    uint32_t hash_id = MurMurHash2(rpc_client.c_str(), rpc_client.size());

    ReadLockGuard rguard(vmap_rwlock_);
    VN_HASH_MAP::iterator vn_iter = vn_map_ptr_->lower_bound(hash_id);
    if (vn_map_ptr_->end() == vn_iter) {
        return false;
    }
    VN_PTR& vn = vn_iter->second;
    vector<string>& py_vec = vn->py_node_list;

    for (vector<string>::iterator iter = py_vec.begin(); iter != py_vec.end(); ++iter) {
        rpc_server_list.push_back(*iter);
    }
    return true;
}

bool ConsistentHashLoadBalancer::BuildConsistentHashMap() {
    return true;
}


}  // end of namespace libevrpc












/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
