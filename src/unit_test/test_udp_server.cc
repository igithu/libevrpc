/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file test_udp_server.cc
 * @author aishuyu(asy5178@163.com)
 * @date 2016/05/25 19:52:16
 * @brief
 *
 **/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include <string>
#include <string.h>

#include "test_def.h"
#include "../util/rpc_communication.h"

using namespace libevrpc;

int main() {

    int32_t server_fd = UdpServerInit(ADDR, PORT);

    std::string recv_info;
    RpcRecvFrom(server_fd, recv_info, true);

    printf("Test recv_info is :%s\n", recv_info.c_str());

    return 0;
}












/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
