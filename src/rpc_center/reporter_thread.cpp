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

#include <string>

#include "rpc_center.h"
#include "util/rpc_communication.h"
#include "util/rpc_util.h"

namespace libevrpc {

using std::string;

ReporterThread::ReporterThread():
    leader_addr_(NULL),
    leader_port_(NULL),
    reporter_running_(false) {
}

ReporterThread::~ReporterThread() {
    if (NULL != leader_addr_) {
        free(leader_addr_);
    }
    if (NULL != leader_port_) {
        free(leader_port_);
    }
}

bool ReporterThread::StartReporter(
        const char* leader_addr,
        const char* leader_port) {
    if (reporter_running_) {
        if (NULL != leader_addr_) {
            free(leader_addr_);
        }
        if (NULL != leader_port_) {
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

    int32_t failed_cnt = 10, retry_time = 3;
    while (reporter_running_) {
        conn_fd_ = TcpConnect(leader_addr_, leader_port_, 15);

        if (failed_cnt > 0 && !rc.ReporterProcessor(conn_fd_)) {
            --failed_cnt;
        }
        if (failed_cnt <= 0) {
            /*
             * 重新连接
             */
            conn_fd_ = TcpConnect(leader_addr_, leader_port_, 15);
            if (conn_fd_ < 0) {
                --retry_time;
            } else {
                /*
                 * 重试连接成功恢复初始状态
                 */
                failed_cnt = 10;
                retry_time = 3;
            }

            if (retry_time <= 0) {
                /*
                 * Leader彻底无法连接 开始新一轮选举 并且当前线程退出
                 */
                rc.StartFastLeaderElection();
                close(conn_fd_);
                break;
            }
        }
        close(conn_fd_);
        sleep(10);
    }
}

}  // end of namespace libevrpc








/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
