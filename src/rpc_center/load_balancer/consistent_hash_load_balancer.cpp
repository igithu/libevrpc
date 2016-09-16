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
    py_server_list_ptr_(new PN_PTR()) {
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
    int32_t virtual_node_num_ = config_parser_instance.IniGetInt("rpc_center:virtual_node_num", 20);
    return true;
}

void ConsistentHashLoadBalancer::SetConfigFile(const std::string& file_name) {
    config_file_ = file_name;
}

bool ConsistentHashLoadBalancer::AddRpcServer(const RpcClusterServer& rpc_server) {
    PN_PTR rcs_ptr(new RpcClusterServer());
    rcs_ptr->CopyFrom(rpc_center);
    py_server_list_ptr_->push_back(rcs_ptr);

    for (int32_t i = 0; i < virtual_node_num_; ++i) {
        vn_map_ptr_->insert(std::make_pair(MurMurHash2("SHARD-" + rcs_ptr->cluster_server_addr() + "-NODE-" + i), rcs_ptr));
    }
    return true;
}

void ConsistentHashLoadBalancer::GetRpcServer(
        const string& rpc_client, vector<string>& rpc_server_list) {
}

bool ConsistentHashLoadBalancer::BuildConsistentHashMap() {
    return true;
}


}  // end of namespace libevrpc












/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
