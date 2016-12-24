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


#include "../rpc_client/client_rpc_controller.h"

int main {
    CenterClientHeartbeat* center_client_heartbeat_ptr = new CenterClientHeartbeat("../test_conf/test.ini");
    center_client_heartbeat_ptr->Start();

    sleep(30);

    center_client_heartbeat_ptr->Stop();
    center_client_heartbeat_ptr->Wait();

    delete center_client_heartbeat_ptr
    return 0;
}












/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
