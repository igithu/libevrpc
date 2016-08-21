/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file reporter_thread.cpp
 * @author aishuyu(asy5178@163.com)
 * @date 2016/07/17 00:43:25
 * @brief
 *
 **/

#include "reporter_thread.h"

#include "rpc_center.h"
#include "util/rpc_communication.h"

namespace libevrpc {

ReporterThread::ReporterThread():
    leader_addr_(NULL),
    leader_port_(NULL),
    reporter_running_(false) {
}

ReporterThread::~ReporterThread() {
    if (NULL != leader_addr) {
        free(leader_addr_);
    }
    if (NULL != leader_port) {
        free(leader_port_);
    }
}

bool StartReporter(
        const char* leader_addr,
        const char* leader_port) {
    if (reporter_running_) {
        if (NULL != leader_addr) {
            free(leader_addr_);
        }
        if (NULL != leader_port) {
            free(leader_port_);
        }
        close(conn_fd_);
        Stop();
    }

    leader_addr_ = (char*)malloc(strlen(leader_addr));
    leader_port_ = (char*)malloc(strlen(leader_port));
    reporter_running_ = true;
    Start();

    return true;
}

void ReporterThread::Run() {

    if (!reporter_running_) {
        return;
    }

    RpcCenter& rc = RpcCenter::GetInstance(g_config_file);
    const char* local_addr = GetLocalAddress();
    CentersProto cp;
    cp.set_from_center_addr(local_addr);
    cp.set_center_action(FOLLOWER_PING);

    conn_fd_ = TcpConnect(leader_addr_, leader_port_);
    while (reporter_running_) {
        string ping_str;
        if (cp.SerializeToString(ping_str)) {
            RpcSend(fd, CENTER2CENTER, ping_str, false);
        }
        sleep(5);
    }
    close(conn_fd_);
}

}  // end of namespace libevrpc








/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
