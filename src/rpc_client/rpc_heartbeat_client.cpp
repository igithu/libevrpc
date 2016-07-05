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

#include "util/rpc_util.h"
#include "util/rpc_communication.h"

namespace libevrpc {

using std::string;

RpcHeartbeatClient::RpcHeartbeatClient(
        const char* hb_server_addr, const char* hb_server_port, int32_t timeout) :
    hb_server_addr_(NULL),
    hb_server_port_(NULL),
    running_(true) {

    if (NULL != hb_server_addr_ && NULL != hb_server_port_) {
        hb_server_addr_ = (char*)malloc(strlen(hb_server_addr));
        hb_server_port_ = (char*)malloc(strlen(hb_server_port));
        strcpy(hb_server_addr_, hb_server_addr);
        strcpy(hb_server_port_, hb_server_port);
    }

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

bool RpcHeartbeatClient::CreateRpcConnection() {
    /*
    int32_t try_times = 3;
    do {
        connect_fd_ = TcpConnect(hb_server_addr_, hb_server_port_, timeout_);
        if (TCP_CONN_TIMEOUT != connect_fd_) {
            break;
        }
        --try_times;
        PrintErrorInfo("Heart beat tcp connect timeout! try again!");
    } while (try_times <= 0);

    if (connect_fd_ < 0) {
        return false;
    }
    */

    /*
     * UDP connection
     */
    if (NULL == hb_server_addr_ || NULL == hb_server_port_) {
        return false;
    }
    connect_fd_ = UdpClientInit(hb_server_addr_, hb_server_port_, to_);
    if (connect_fd_ < 0) {
        return false;
    }

    return true;
}

void RpcHeartbeatClient::Run() {
    if (!CreateRpcConnection()) {
        PrintErrorInfo("Create the rpc heartbeat connection failed! Heartbeat thread exit!");
        return;
    }
    /*
     * send local address to server
     */
    string ping = GetLocalAddress();
    while (running_) {
        if (RpcSendTo(connect_fd_, to_, ping, false) < 0) {
            PrintErrorInfo("Rpc Heartbeat send info failed!");
            sleep(3);
            continue;
        }
        sleep(10);
    }
    close(connect_fd_);
}


}  // end of namespace libevrpc












/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
