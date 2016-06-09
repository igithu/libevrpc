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
#include "util/rpc_util.h"
#include "util/rpc_communication.h"

using namespace libevrpc;

int main() {
    ConnectionTimerManager& ctm = ConnectionTimerManager::GetInstance("test_conf/test.ini");
    int32_t buf_index = ctm.InitTimerBuf();
    ctm.Start();

    printf("Test start connect\n");
    const char* addr = GetLocalAddress();
    printf("Test the addr is : %s\n", addr);

    int32_t listen_fd = TcpListen(addr, "7777", false);

    struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    printf("Test the listen_fd is : %d\n", listen_fd);
    int32_t cfd = Accept(listen_fd, client_addr, len, false);
    if (cfd < 0) {
        printf("accpet eror!\n");
        return 0;
    }
    std::string guest_addr;
    GetPeerAddr(cfd, guest_addr);
    ctm.InsertConnectionTimer(guest_addr, cfd, buf_index, 1, pthread_self());
    printf("InsertConnectionTimer done!\n");

    printf("Start done!\n");

    sleep(200);
    std::string  recv_msg;
    int id = RpcRecv(cfd, recv_msg, true);
    printf("Test string is :%s\n", recv_msg.c_str());
    sleep(1000);

    ctm.Stop();

    return 0;
}










/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
