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


#include "rpc_center/load_balancer/consistent_hash_load_balancer.h"

#include "center_proto/center_cluster.pb.h"
#include "config_parser/config_parser.h"
#include "util/rpc_util.h"


namespace libevrpc {

using std::string;
using std::vector;

ConsistentHashLoadBalancer::ConsistentHashLoadBalancer(const string& config_file) : LoadBalancer(config_file){
    // :
    // config_file_(config_file),
    // virtual_node_num_(20),
    // vn_map_ptr_(new VN_HASH_MAP()) {

    config_file_ = config_file;
    virtual_node_num_ =20;
    vn_map_ptr_ = new VN_HASH_MAP();
}

ConsistentHashLoadBalancer::~ConsistentHashLoadBalancer() {
    if (NULL != vn_map_ptr_) {
        delete vn_map_ptr_;
    }
    // if (NULL != py_server_list_ptr_) {
    //     delete py_server_list_ptr_;
    // }
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

bool ConsistentHashLoadBalancer::AddRpcServer(const string& rpc_server) {
    WriteLockGuard wguard(vmap_rwlock_);
    for (int32_t i = 0; i < virtual_node_num_; ++i) {
        string hash_str = "SHARD-" + rpc_server + "-NODE-" + static_cast<char>(i);
        uint32_t hash_id = MurMurHash2(hash_str.c_str(), hash_str.size());
        vn_map_ptr_->insert(std::make_pair(hash_id, rpc_server));
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
    rpc_server_list.push_back(vn_iter->second);
    return true;
}

bool ConsistentHashLoadBalancer::BuildConsistentHashMap() {
    return true;
}


}  // end of namespace libevrpc












/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
