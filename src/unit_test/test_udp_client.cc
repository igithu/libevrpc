/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file test_udp_client.cc
 * @author aishuyu(asy5178@163.com)
 * @date 2016/05/25 20:08:24
 * @brief
 *
 **/

#include <stdio.h>
#include <unistd.h>

#include <string>
#include <string.h>
#include <errno.h>

#include "test_def.h"
#include "../util/rpc_communication.h"

using namespace libevrpc;

int main() {

    struct sockaddr_in to;
    int32_t conn_fd = UdpClientInit(ADDR, PORT, to);

    printf("Test UdpClientInit done! conn_fd is %d error no is %s\n", conn_fd, strerror(errno));

    std::string send_info = "udp Test123456789";
    int32_t ret = RpcSendTo(conn_fd, to, send_info, true);
    if (ret < 0) {
        printf("RpcSendTo error\n");
    } else {
        printf("RpcSendTo sucessful ret is %d\n", ret);
    }

    sleep(200);

    return 0;
}












/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
