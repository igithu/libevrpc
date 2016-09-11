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


namespace libevrpc {

using std::string;
using std::vector;

ConsistentHashLoadBalancer::ConsistentHashLoadBalancer() : config_file_("") {
}

ConsistentHashLoadBalancer::~ConsistentHashLoadBalancer() {
}

bool ConsistentHashLoadBalancer::InitBalancer() {
    if ("" == config_file_) {
        return false;
    }
    return true;
}

void ConsistentHashLoadBalancer::SetConfigFile(const std::string& file_name) {
    config_file_ = file_name;
}

bool ConsistentHashLoadBalancer::AddRpcServer(const string& rpc_server) {
    return true;
}

void ConsistentHashLoadBalancer::GetRpcServer(
        const string& rpc_client, vector<string>& rpc_server_list) {
}



}  // end of namespace libevrpc












/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
