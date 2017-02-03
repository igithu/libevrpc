/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file center_test.cc
 * @author aishuyu(asy5178@163.com)
 * @date 2016/12/22 21:57:08
 * @brief
 *
 **/

#include "../rpc_center/rpc_center.h"

using namespace libevrpc;

int main() {
    RpcCenter& rc = RpcCenter::GetInstance("test_conf/rpc_center.ini");
    printf("Start Center........\n");
    if (!rc.StartCenter()) {
        printf("Start Center Failed\n");
        return 0;
    }
    printf("Wait Center........\n");
    rc.WaitCenter();
    printf("Stop Center........\n");
    rc.StopCenter();
    return 0;
}











/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
