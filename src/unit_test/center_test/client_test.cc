/***************************************************************************
 *
 * Copyright (c) 2016 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file client_test.cc
 * @author aishuyu(asy5178@163.com)
 * @date 2016/12/22 21:36:33
 * @brief
 *
 **/


#include "../rpc_client/center_client_heartbeat.h"

using namespace libevrpc;


int main() {

    CenterClientHeartbeat center_client_heartbeat("test_conf/test.ini");
    center_client_heartbeat.Start();

    sleep(30);

    center_client_heartbeat.Stop();
    center_client_heartbeat.Wait();

//    delete center_client_heartbeat_ptr
    return 0;
}












/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
