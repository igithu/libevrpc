/***************************************************************************
 *
 * Copyright (c) 2017 aishuyu, Inc. All Rights Reserved
 *
 **************************************************************************/



/**
 * @file cluster_test.cc
 * @author aishuyu(asy5178@163.com)
 * @date 2017/03/10 20:47:59
 * @brief
 *
 **/



#include <unistd.h>
#include <time.h>

#include "../rpc_server/center_cluster_heartbeat.h"

using namespace libevrpc;


int main() {

    CenterClusterHeartbeat center_cluster_heartbeat("test_conf/test.ini");
    center_cluster_heartbeat.Start();

    sleep(30);

    center_cluster_heartbeat.Stop();
    center_cluster_heartbeat.Wait();

    return 0;
}














/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
