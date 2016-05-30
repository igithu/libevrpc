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

#include <unistd.h>

#include "util/rpc_util.h"
#include "util/rpc_communication.h"
#include "rpc_client/rpc_heartbeat_client.h"

using namespace libevrpc;

int main() {
    int32_t conn_fd = TcpConnect(GetLocalAddress(), "7777", 10000);
    //int32_t conn_fd = TcpConnect("127.0.0.1", "7777", 10000);

    if (conn_fd < 0) {
        printf("Tcp conncect error!\n");
        return 0;
    }
    RpcHeartbeatClient rhc(GetLocalAddress() ,"9999", 1000);
    // RpcHeartbeatClient rhc("127.0.0.1", "9999", 1000);
    printf("RpcHeartbeatClient start\n");
    rhc.Start();

    std::string test_str = "123start12345678901234567890end123";

    // int32_t ret = RpcSend(conn_fd, 0, test_str, true);
    printf("RpcHeartbeatClient start\n");
    // printf("ret id is %d\n", ret);

    printf("RpcHeartbeatClient sleep\n");
    sleep(50);
    rhc.Stop();
    rhc.Wait();
    return 0;
}











/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
