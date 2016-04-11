/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
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

#include <string.h>
#include <unistd.h>

#include "util/rpc_communication.h"

namespace libevrpc {

RpcHeartbeatClient::RpcHeartbeatClient(
        const char* hb_server_addr, const char* hb_server_port, int32_t timeout) :
    hb_server_addr_(NULL),
    hb_server_port_(NULL),
    running_(true) {

    hb_server_addr_ = (char*)malloc(strlen(hb_server_addr));
    hb_server_port_ = (char*)malloc(strlen(hb_server_port));
    strcpy(hb_server_addr_, hb_server_addr);
    strcpy(hb_server_port_, hb_server_port);

    timeout_ = timeout;
}

RpcHeartbeatClient::~RpcHeartbeatClient() {
    if (NULL != hb_server_addr_) {
        free(hb_server_addr_);
    }
    if (NULL != hb_server_port_) {
        free(hb_server_port_);
    }
}

int32_t RpcHeartbeatClient::InitRpcConnection() {
    int32_t try_times = 3;
    do {
        connect_fd_ = TcpConnect(hb_server_addr_, hb_server_port_, timeout_);
        if (TCP_CONN_TIMEOUT != connect_fd_) {
            break;
        }
        --try_times;
        fprintf(stderr, "TcpConnect timeout! try again\n");
    } while (try_times <= 0);

    return 0;
}

void RpcHeartbeatClient::Run() {
    InitRpcConnection();

    while (running_) {
        sleep(5);
    }
}


}  // end of namespace libevrpc












/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
