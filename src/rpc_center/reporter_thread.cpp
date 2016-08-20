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
        Stop();
    }

    leader_addr_ = (char*)malloc(strlen(leader_addr));
    leader_port_ = (char*)malloc(strlen(leader_port));
    reporter_running_ = true;
    Start();

    return true;
}

void ReporterThread::Run() {

    RpcCenter& rc = RpcCenter::GetInstance(g_config_file);

    while (reporter_running_) {
        sleep(5);
    }
}

}  // end of namespace libevrpc








/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
