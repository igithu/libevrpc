/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file test_hb_server.cc
 * @author aishuyu(asy5178@163.com)
 * @date 2016/05/08 21:41:40
 * @brief
 *
 **/

#include <unistd.h>

#include "rpc_server/connection_timer_manager.h"
#include "rpc_server/rpc_heartbeat_server.h"

using namespace libevrpc;

int main() {
    ConnectionTimerManager& ctm = ConnectionTimerManager::GetInstance();
    RpcHeartbeatServer rhs("127.0.0.1", "9999");

    ctm.Start();
    rhc.Start();

    sleep(1000);

    ctm.Stop();
    rhc.Stop();

    return 0;
}










/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
