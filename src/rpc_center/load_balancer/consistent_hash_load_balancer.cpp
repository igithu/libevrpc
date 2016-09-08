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

ConsistentHashLoadBalancer::ConsistentHashLoadBalancer() {
}

ConsistentHashLoadBalancer::~ConsistentHashLoadBalancer() {
}

bool ConsistentHashLoadBalancer::InitBalancer() {
    return true;
}

bool ConsistentHashLoadBalancer::AddRpcServer(const string& rpc_server) {
    return true;
}

string ConsistentHashLoadBalancer::GetRpcServer(const string& rpc_client) {
    return "";
}



}  // end of namespace libevrpc












/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
