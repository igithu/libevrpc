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

#include "rpc_center.h"

using namespace libevrpc;

int main() {
    RpcCenter& rc = RpcCenter::GetInstance("conf/rpc_center.ini");
    rc.StartCenter();
    rc.WaitCenter();
    rc.StopCenter();
    return 0;
}











/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
