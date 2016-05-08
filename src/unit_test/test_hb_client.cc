/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file test_hb_client.cc
 * @author aishuyu(asy5178@163.com)
 * @date 2016/05/08 21:33:56
 * @brief
 *
 **/

#include "rpc_client/rpc_heartbeat_client.h"

using namespace libevrpc;

int main() {
    RpcHeartbeatClient rhc("127.0.0.1" ,"9999", 20);
    rhc.Start();

    sleep(50);
    rhc.Stop();
    rhc.Wait();
    return 0;
}











/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
