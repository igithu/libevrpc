/***************************************************************************
 *
 * Copyright (c) 2015 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file rpc_heartbeat_client.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2016/04/04 21:13:38
 * @brief
 *
 **/

#include "rpc_heartbeat_client.h"

namespace libevrpc {

RpcHeartbeatClient::RpcHeartbeatClient() : running_(true) {
}

RpcHeartbeatClient::~RpcHeartbeatClient() {
}

int32_t RpcHeartbeatClient::InitRpcConnection() {
}

void RpcHeartbeatClient::Run() {
    InitRpcConnection();

    while (running_) {
        sleep(5);
    }
}


}  // end of namespace libevrpc












/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
